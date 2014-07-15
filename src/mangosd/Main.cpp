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

#include "AuctionHouseBot/AuctionHouseBot.h"
#include "CliRunnable.h"
#include "Common.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "FreezeDetector.h"
#include "Log.h"
#include "MassMailMgr.h"
#include "MaNGOSsoap.h"
#include "MapManager.h"
#include "ProgressBar.h"
#include "RemoteAdministration.h"
#include "revision.h"
#include "revision_nr.h"
#include "revision_sql.h"
#include "SystemConfig.h"
#include "Util.h"
#include "WorldSocketMgr.h"
#include "helper/ConsoleArguments.h"
#include "helper/Hints.h"
#include "helper/ProcessPriority.h"

const uint32 kWorldSleepTime = 50;

boost::asio::io_service IoService;

DatabaseType WorldDatabase;
DatabaseType CharacterDatabase;
DatabaseType LoginDatabase;

uint32 realmID;

// Clear 'online' status for all accounts with characters in this realm
void ClearOnlineAccounts()
{
    // Cleanup online status for characters hosted at current realm
    // \todo Only accounts with characters logged on *this* realm should have online status reset. Move the online column from 'account' to 'realmcharacters'?
    LoginDatabase.PExecute("UPDATE account SET active_realm_id = 0 WHERE active_realm_id = '%u'", realmID);

    CharacterDatabase.Execute("UPDATE characters SET online = 0 WHERE online<>0");

    // Battleground instance ids reset at server restart
    CharacterDatabase.Execute("UPDATE character_battleground_data SET instance_id = 0");
}

void SignalHandler(const boost::system::error_code& error, int signal_number)
{
    if (!error)
    {
        if (signal_number == SIGINT)
            World::StopNow(RESTART_EXIT_CODE);
        else
            World::StopNow(SHUTDOWN_EXIT_CODE);
    }
}

bool StartDatabase()
{
    // Get world database info from configuration file
    std::string dbstring = sConfig.GetStringDefault("WorldDatabaseInfo", "");
    int nConnections = sConfig.GetIntDefault("WorldDatabaseConnections", 1);
    if (dbstring.empty())
    {
        sLog.outError("Database not specified in configuration file");
        return false;
    }
    sLog.outString("World Database total connections: %i", nConnections + 1);

    // Initialise the world database
    if (!WorldDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to world database %s", dbstring.c_str());
        return false;
    }

    if (!WorldDatabase.CheckRequiredField("db_version", REVISION_DB_MANGOS))
    {
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }

    dbstring = sConfig.GetStringDefault("CharacterDatabaseInfo", "");
    nConnections = sConfig.GetIntDefault("CharacterDatabaseConnections", 1);
    if (dbstring.empty())
    {
        sLog.outError("Character Database not specified in configuration file");

        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }
    sLog.outString("Character Database total connections: %i", nConnections + 1);

    // Initialise the Character database
    if (!CharacterDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to Character database %s", dbstring.c_str());

        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        return false;
    }

    if (!CharacterDatabase.CheckRequiredField("character_db_version", REVISION_DB_CHARACTERS))
    {
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }

    // Get login database info from configuration file
    dbstring = sConfig.GetStringDefault("LoginDatabaseInfo", "");
    nConnections = sConfig.GetIntDefault("LoginDatabaseConnections", 1);
    if (dbstring.empty())
    {
        sLog.outError("Login database not specified in configuration file");

        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }

    // Initialise the login database
    sLog.outString("Login Database total connections: %i", nConnections + 1);
    if (!LoginDatabase.Initialize(dbstring.c_str(), nConnections))
    {
        sLog.outError("Cannot connect to login database %s", dbstring.c_str());

        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        return false;
    }

    if (!LoginDatabase.CheckRequiredField("realmd_db_version", REVISION_DB_REALMD))
    {
        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        LoginDatabase.HaltDelayThread();
        return false;
    }

    // Get the realm Id from the configuration file
    realmID = sConfig.GetIntDefault("RealmID", 0);
    if (!realmID)
    {
        sLog.outError("Realm ID not defined in configuration file");

        // Wait for already started DB delay threads to end
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        LoginDatabase.HaltDelayThread();
        return false;
    }

    sLog.outString("Realm running as realm ID %d", realmID);

    // Clean the database before starting
    ClearOnlineAccounts();

    sWorld.LoadDBVersion();

    sLog.outString("Using World DB: %s", sWorld.GetDBVersion());
    sLog.outString("Using creature EventAI: %s", sWorld.GetCreatureEventAIVersion());
    return true;
}

void WorldUpdateLoop()
{
    uint32 realCurrTime = 0;
    uint32 realPrevTime = WorldTimer::tick();
    uint32 prevSleepTime = 0; // Used for balanced full tick time length near WORLD_SLEEP_CONST

    // While we have not World::m_stopEvent, update the world
    while (!World::IsStopped())
    {
        ++World::m_worldLoopCounter;
        realCurrTime = WorldTimer::getMSTime();

        uint32 diff = WorldTimer::tick();

        sWorld.Update(diff);
        realPrevTime = realCurrTime;

        // diff (D0) include time of previous sleep (d0) + tick time (t0)
        // we want that next d1 + t1 == kWorldSleepTime
        // we can't know next t1 and then can use (t0 + d1) == kWorldSleepTime requirement
        // d1 = kWorldSleepTime - t0 = kWorldSleepTime - (D0 - d0) = kWorldSleepTime + d0 - D0
        if (diff <= kWorldSleepTime + prevSleepTime)
        {
            prevSleepTime = kWorldSleepTime + prevSleepTime - diff;
            MaNGOS::Thread::Sleep(prevSleepTime);
        }
        else
            prevSleepTime = 0;
    }
}

int main(int ac, char* av[])
{
    using namespace cmangos::helper;

    std::string configuration_file = _MANGOSD_CONFIG;
    ConsoleArguments console_arguments(configuration_file);
    ConsoleArguments::Options options;
    options.add_options()
        ("ahbot,a", boost::program_options::value<std::string>(), "use as ahbot configuration file")
        ;
    console_arguments.AddOptions(options);
    console_arguments.Parse(ac, av);
    ConsoleArguments::Variables variables = console_arguments.Get();

    if (variables.count("help"))
    {
        console_arguments.PrintAllowedOptions();
        return 0;
    }

    if (variables.count("ahbot"))
        sAuctionBotConfig.SetConfigFileName(variables["ahbot"].as<std::string>().c_str());

    if (!sConfig.SetSource(configuration_file, "MangosdConf"))
    {
        sLog.outError("Could not find configuration file %s.", configuration_file.c_str());
        return 1;
    }

    CheckConfigurationFileVersion(_MANGOSDCONFVERSION);
    CmangosHint("WORLD");
    BoostHint();
    OpensslHint();

    // Set progress bars show mode
    BarGoLink::SetOutputState(sConfig.GetBoolDefault("ShowProgressBars", true));

    // Start the databases
    if (!StartDatabase())
    {
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    // Initialize the World
    sWorld.SetInitialWorldSettings();

    // Server loaded successfully => enable async DB requests
    // This is done to forbid any async transactions during server startup!
    CharacterDatabase.AllowAsyncTransactions();
    WorldDatabase.AllowAsyncTransactions();
    LoginDatabase.AllowAsyncTransactions();

    // Register a signal handler to catch shutdown event.
    boost::asio::signal_set signals(IoService, SIGINT, SIGTERM);
    signals.async_wait(SignalHandler);

    // Set process priority.
    SetProcessPriority(sConfig.GetIntDefault("UseProcessors", 0), sConfig.GetBoolDefault("ProcessPriority", false));

    // Set realmbuilds depend on mangosd expected builds, and set server online
    {
        std::string builds = AcceptableClientBuildsListStr();
        LoginDatabase.escape_string(builds);
        LoginDatabase.DirectPExecute("UPDATE realmlist SET realmflags = realmflags & ~(%u), population = 0, realmbuilds = '%s'  WHERE id = '%u'", REALM_FLAG_OFFLINE, builds.c_str(), realmID);
    }

    // Launch CliRunnable thread
    MaNGOS::Thread* cliThread = NULL;
    if (sConfig.GetBoolDefault("Console.Enable", true))
        cliThread = new MaNGOS::Thread(new CliRunnable);

    MaNGOS::Thread* rar_thread = NULL;
    if (sConfig.GetBoolDefault("Ra.Enable", false))
        rar_thread = new MaNGOS::Thread(new RARunnable);

    // Start soap serving thread
    MaNGOS::Thread* soap_thread = NULL;

    if (sConfig.GetBoolDefault("SOAP.Enabled", false))
    {
        MaNGOSsoapRunnable* runnable = new MaNGOSsoapRunnable();

        runnable->setListenArguments(sConfig.GetStringDefault("SOAP.IP", "127.0.0.1"), sConfig.GetIntDefault("SOAP.Port", 7878));
        soap_thread = new MaNGOS::Thread(runnable);
    }

    // Start up freeze catcher thread
    MaNGOS::Thread* freeze_thread = NULL;
    if (uint32 freeze_delay = sConfig.GetIntDefault("MaxCoreStuckTime", 0))
    {
        FreezeDetectorRunnable* fdr = new FreezeDetectorRunnable();
        fdr->SetDelayTime(freeze_delay * 1000);
        freeze_thread = new MaNGOS::Thread(fdr);
        freeze_thread->setPriority(MaNGOS::Priority_Highest);
    }

    // Launch the world listener socket
    uint16 wsport = sWorld.getConfig(CONFIG_UINT32_PORT_WORLD);
    std::string bind_ip = sConfig.GetStringDefault("BindIP", "0.0.0.0");

    if (!sWorldSocketMgr.StartNetwork(wsport, bind_ip))
    {
        sLog.outError("Failed to start network");
        Log::WaitBeforeContinueIfNeed();
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }

    // Init new SQL thread for the world database
    WorldDatabase.ThreadStart();                            // let thread do safe mySQL requests (one connection call enough)
    sWorld.InitResultQueue();

    WorldUpdateLoop();

    IoService.stop();
    sWorld.CleanupsBeforeStop();
    sWorldSocketMgr.StopNetwork();
    MapManager::Instance().UnloadAll();                     // unload all grids (including locked in memory)

    // End the database thread
    WorldDatabase.ThreadEnd();                              // free mySQL thread resources

    // Stop freeze protection before shutdown tasks
    if (freeze_thread)
    {
        freeze_thread->destroy();
        delete freeze_thread;
    }

    // Stop soap thread
    if (soap_thread)
    {
        soap_thread->wait();
        soap_thread->destroy();
        delete soap_thread;
    }

    // Set server offline in realmlist
    LoginDatabase.DirectPExecute("UPDATE realmlist SET realmflags = realmflags | %u WHERE id = '%u'", REALM_FLAG_OFFLINE, realmID);

    if (rar_thread)
    {
        rar_thread->wait();
        rar_thread->destroy();
        delete rar_thread;
    }

    // Clean account database before leaving
    ClearOnlineAccounts();

    // send all still queued mass mails (before DB connections shutdown)
    sMassMailMgr.Update(true);

    // Wait for DB delay threads to end
    CharacterDatabase.HaltDelayThread();
    WorldDatabase.HaltDelayThread();
    LoginDatabase.HaltDelayThread();

    sLog.outString("Halting process...");

    if (cliThread)
    {
#ifdef WIN32
        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[5];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;

        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;

        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;

        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        BOOL ret = WriteConsoleInput(hStdIn, b, 4, &numb);

        cliThread->wait();
#else
        cliThread->destroy();
#endif
        delete cliThread;
    }

    // Exit the process with specified return value
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart mangosd
    return World::GetExitCode(); 
}