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

/// \addtogroup mangosd Mangos Daemon
/// @{
/// \file

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Config/Config.h"
#include "ProgressBar.h"
#include "Log.h"
#include "Master.h"
#include "SystemConfig.h"
#include "AuctionHouseBot/AuctionHouseBot.h"

#include "revision.h"
#include "revision_nr.h"

#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include <ace/Version.h>
#include <boost/program_options.hpp>
#include <boost/version.hpp>
#include <sstream>

#ifdef WIN32
#include "ServiceWin32.h"
char serviceName[] = "mangosd";
char serviceLongName[] = "MaNGOS world service";
char serviceDescription[] = "Massive Network Game Object Server";
/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#else
#include "PosixDaemon.h"
#endif

DatabaseType WorldDatabase;                                 ///< Accessor to the world database
DatabaseType CharacterDatabase;                             ///< Accessor to the character database
DatabaseType LoginDatabase;                                 ///< Accessor to the realm/login database

uint32 realmID;                                             ///< Id of the realm

/// Print out the usage string for this program on the console.
void usage(boost::program_options::options_description const& desc, const char* prog)
{
    std::ostringstream ss;
    ss << desc;
    sLog.outString("Usage: \n %s [<options>]\n%s", prog, ss.str().c_str());
}

/// Launch the mangos server
extern int main(int argc, char** argv)
{
    ///- Command line parsing
    std::string cfg_file;
    std::string serviceDaemonMode;

    boost::program_options::options_description description("Allowed options");
    description.add_options()
        ("version,v", "print version and exit")
        ("help,h", "print commandline help and exit")
        ("config,c", boost::program_options::value<std::string>(&cfg_file)->default_value(_MANGOSD_CONFIG), "use as configuration file")
        ("ahbot,a", boost::program_options::value<std::string>(), "use as ahbot configuration file")
#ifdef WIN32
        ("service,s", boost::program_options::value<std::string>(&serviceDaemonMode), "running as service, arg functions: run, install, uninstall")
#else
        ("service,s", boost::program_options::value<std::string>(&serviceDaemonMode), "running as daemon, arg functions: run, stop")
#endif
        ;

    // parse option
    boost::program_options::variables_map vm;

    try {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
        options(description).run(), vm);
        boost::program_options::notify(vm);
    }
    catch(boost::program_options::unknown_option const& ex)
    {
        sLog.outError("Runtime-Error: unknown option %s", ex.get_option_name().c_str());
        usage(description, argv[0]);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }
    catch(boost::program_options::invalid_command_line_syntax const& ex)
    {
        sLog.outError("Runtime-Error: invalid syntax for option %s", ex.get_option_name().c_str());
        usage(description, argv[0]);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

    if (vm.count("version"))
    {
        printf("%s\n", _FULLVERSION(REVISION_DATE, REVISION_TIME, REVISION_NR, REVISION_ID));
        return 0;
    }

    if (vm.count("help"))
    {
        usage(description, argv[0]);
        return 0;
    }

    if (vm.count("ahbot"))
        sAuctionBotConfig.SetConfigFileName(vm["ahbot"].as<std::string>().c_str());

    if (!serviceDaemonMode.empty())
    {
#ifdef WIN32
        char const* const serviceModes[] =  { "run", "install", "uninstall", NULL };
#else
        char const* const serviceModes[] =  { "run", "stop", NULL };
#endif
        char const* const* mode_ptr = &serviceModes[0];
        for(; *mode_ptr != NULL; ++mode_ptr)
            if (*mode_ptr == serviceDaemonMode)
                break;

        if (!*mode_ptr)
        {
            sLog.outError("Runtime-Error: -s unsupported argument %s", serviceDaemonMode.c_str());
            usage(description, argv[0]);
            Log::WaitBeforeContinueIfNeed();
            return 1;
        }
    }

#ifdef WIN32                                                // windows service command need execute before config read
    switch (serviceDaemonMode[0])
    {
        case 'i':
            if (WinServiceInstall())
                sLog.outString("Installing service");
            return 1;
        case 'u':
            if (WinServiceUninstall())
                sLog.outString("Uninstalling service");
            return 1;
        case 'r':
            WinServiceRun();
            break;
    }
#endif

    if (!sConfig.SetSource(cfg_file, "MangosdConf"))
    {
        sLog.outError("Could not find configuration file %s.", cfg_file.c_str());
        usage(description, argv[0]);
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

#ifndef WIN32                                               // posix daemon commands need apply after config read
    switch (serviceDaemonMode[0])
    {
        case 'r':
            startDaemon();
            break;
        case 's':
            stopDaemon();
            break;
    }
#endif

    sLog.outString("%s [world-daemon]", _FULLVERSION(REVISION_DATE, REVISION_TIME, REVISION_NR, REVISION_ID));
    sLog.outString("<Ctrl-C> to stop.");
    sLog.outString("\n\n"
                   "MM   MM         MM   MM  MMMMM   MMMM   MMMMM\n"
                   "MM   MM         MM   MM MMM MMM MM  MM MMM MMM\n"
                   "MMM MMM         MMM  MM MMM MMM MM  MM MMM\n"
                   "MM M MM         MMMM MM MMM     MM  MM  MMM\n"
                   "MM M MM  MMMMM  MM MMMM MMM     MM  MM   MMM\n"
                   "MM M MM M   MMM MM  MMM MMMMMMM MM  MM    MMM\n"
                   "MM   MM     MMM MM   MM MM  MMM MM  MM     MMM\n"
                   "MM   MM MMMMMMM MM   MM MMM MMM MM  MM MMM MMM\n"
                   "MM   MM MM  MMM MM   MM  MMMMMM  MMMM   MMMMM\n"
                   "        MM  MMM http://getmangos.com\n"
                   "        MMMMMM\n\n");
    sLog.outString("Using configuration file %s.", cfg_file.c_str());

    DETAIL_LOG("%s (Library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
    if (SSLeay() < 0x009080bfL)
    {
        DETAIL_LOG("WARNING: Outdated version of OpenSSL lib. Logins to server may not work!");
        DETAIL_LOG("WARNING: Minimal required version [OpenSSL 0.9.8k]");
    }

    DETAIL_LOG("Using ACE: %s", ACE_VERSION);
    DETAIL_LOG("Using BOOST: %i.%i.%i", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);

    ///- Set progress bars show mode
    BarGoLink::SetOutputState(sConfig.GetBoolDefault("ShowProgressBars", true));

    ///- and run the 'Master'
    /// \todo Why do we need this 'Master'? Can't all of this be in the Main as for Realmd?
    return sMaster.Run();

    // at sMaster return function exist with codes
    // 0 - normal shutdown
    // 1 - shutdown at error
    // 2 - restart command used, this code can be used by restarter for restart mangosd
}

/// @}
