/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/thread.hpp>
#include "AuthSocket.h"
#include "Common.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "RealmList.h"
#include "revision.h"
#include "revision_nr.h"
#include "revision_sql.h"
#include "SessionManager.h"
#include "SystemConfig.h"
#include "Util.h"
#include "helper/ConsoleArguments.h"
#include "helper/Hints.h"
#include "helper/ProcessPriority.h"

boost::asio::io_service IoService;

uint32 DatabasePingInterval;
boost::asio::deadline_timer DatabasePingTimer(IoService);

DatabaseType LoginDatabase;

void SignalHandler(const boost::system::error_code& error, int signal_number)
{
    if (!error)
        IoService.stop();
}

void KeepDatabaseAliveHandler(const boost::system::error_code& error)
{
    if (!error)
    {
        DETAIL_LOG("Ping MySQL to keep connection alive");
        LoginDatabase.Ping();

        DatabasePingTimer.expires_from_now(boost::posix_time::minutes(DatabasePingInterval));
        DatabasePingTimer.async_wait(KeepDatabaseAliveHandler);
    }
}

bool StartDatabase()
{
    std::string dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    if (dbstring.empty())
    {
        sLog.outError("Database not specified");
        return false;
    }

    sLog.outString("Login Database total connections: %i", 1 + 1);

    if (!LoginDatabase.Initialize(dbstring.c_str()))
    {
        sLog.outError("Cannot connect to database");
        return false;
    }

    if (!LoginDatabase.CheckRequiredField("realmd_db_version", REVISION_DB_REALMD))
    {
        // Wait for already started DB delay threads to end
        LoginDatabase.HaltDelayThread();
        return false;
    }

    return true;
}

int main(int ac, char* av[])
{
    using namespace cmangos::helper;

    std::string configuration_file = _REALMD_CONFIG;
    ConsoleArguments console_arguments(configuration_file);
    console_arguments.Parse(ac, av);
    ConsoleArguments::Variables variables = console_arguments.Get();

    if (variables.count("help"))
    {
        console_arguments.PrintAllowedOptions();
        return 0;
    }

    if (!sConfig.SetSource(configuration_file, "RealmdConf"))
    {
        sLog.outError("Could not find configuration file %s.", configuration_file);
        return 1;
    }

    sLog.Initialize();

    CheckConfigurationFileVersion(_REALMDCONFVERSION);
    CmangosHint("AUTHENTICATION");
    BoostHint();
    OpensslHint();

    // Initialize the database connection
    if (!StartDatabase())
    {
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    // Get the list of realms for the server
    sRealmList.Initialize(sConfig.GetIntDefault("RealmsStateUpdateDelay", 20));
    if (sRealmList.size() == 0)
    {
        sLog.outError("No valid realms specified.");
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    // Cleanup expired bans before accept connections
    LoginDatabase.BeginTransaction();
    LoginDatabase.Execute("UPDATE account_banned SET active = 0 WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");
    LoginDatabase.CommitTransaction();

    // Launch the listening network socket
    uint16 rmport = sConfig.GetIntDefault("RealmServerPort", DEFAULT_REALMSERVER_PORT);
    std::string bind_ip = sConfig.GetStringDefault("BindIP", "0.0.0.0");

    std::auto_ptr<SessionManager> manager(new SessionManager());
    if (!manager->StartNetwork(rmport, bind_ip))
    {
        sLog.outError("CMaNGOS realmd can not bind to %s:%d", bind_ip.c_str(), rmport);
        return 1;
    }

    // Register a signal handler to catch shutdown event.
    boost::asio::signal_set signals(IoService, SIGINT, SIGTERM);
    signals.async_wait(SignalHandler);
   
    // Set process priority.
    SetProcessPriority(sConfig.GetIntDefault("UseProcessors", 0), sConfig.GetBoolDefault("ProcessPriority", false));

    // Server has started up successfully => enable async DB requests
    LoginDatabase.AllowAsyncTransactions();

    // Enabled a timed callback for handling the database keep alive ping
    DatabasePingInterval = sConfig.GetIntDefault("MaxPingTime", 30);
    DatabasePingTimer.expires_from_now(boost::posix_time::minutes(DatabasePingInterval));
    DatabasePingTimer.async_wait(KeepDatabaseAliveHandler);

    IoService.run();

    sLog.outString("Halting process...");

    manager.reset();

    // Wait for the delay thread to exit
    LoginDatabase.HaltDelayThread();

    return 0;
}