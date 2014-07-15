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

#ifndef HINTS_H_
#define HINTS_H_

#include <boost/asio/socket_base.hpp>
#include <boost/version.hpp>
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include "Common.h"
#include "Log/Log.h"

namespace cmangos {
namespace helper {

void CmangosHint(const char* service_name)
{
    DETAIL_LOG("\n"
        "      ____ __  __       _   _  _____  ____   _____ \n"
        "    / ____|  \\/  |     | \\ | |/ ____|/ __ \\ / ____|\n"
        "   | |    | \\  / |     |  \\| | |  __  |  | | (___  \n"
        "   | |    | |\\/| | __ _| . ` | | |_ | |  | |\\___ \\ \n"
        "   | |____| |  | |/ _` | |\\  | |__| | |__| |____) |\n"
        "    \\_____|_|  |_| (_| |_| \\_|\\_____|\\____/ \\____/ \n"
        "                  \\__,_|\n"
        "                           %s SERVICE\n", service_name);

  DETAIL_LOG("Welcome to C(ontinued)-MaNGOS - Doing WoW-Emulation Right!");
  DETAIL_LOG("Project website: http://cmangos.net.");
}

void BoostHint()
{
    DETAIL_LOG("Using Boost Libraries: %i.%i.%i.", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
    DETAIL_LOG("Max allowed connections are: %d.", boost::asio::socket_base::max_connections);
}

void OpensslHint()
{
    const char* hint = OPENSSL_VERSION_TEXT;
    DETAIL_LOG("Using OpenSSL Toolkit: %s.", hint + 8);
    if (SSLeay() < 0x009080bfL)
        DETAIL_LOG("Outdated version of OpenSSL. Logins to server may not work! Minimal required version [OpenSSL 0.9.8k]");
}

void CheckConfigurationFileVersion(uint32 latest_version)
{
    uint32 configuration_version = sConfig.GetIntDefault("ConfVersion", 0);
    if (configuration_version < latest_version)
    {
        sLog.outError("*****************************************************************************");
        sLog.outError(" WARNING: Your configuration file is outdated!");
        sLog.outError("          Please check for updates, as your current default values may cause");
        sLog.outError("          strange behavior.");
        sLog.outError("*****************************************************************************");
    }
}

} // namespace helper
} // namespace cmangos

#endif // HINTS_H_