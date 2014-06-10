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

#include "RemoteAdministration.h"
#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Log.h"
#include "World.h"
#include "Config/Config.h"
#include "Util.h"
#include "AccountMgr.h"
#include "Language.h"
#include "ObjectMgr.h"

RASocket::RASocket() : RAHandler(), pendingCommands(0, USYNC_THREAD, "pendingCommands"), outActive(false),
    inputBufferLen(0), outputBufferLen(0), stage(NONE)
{
    bSecure = sConfig.GetBoolDefault("RA.Secure", true);
    bStricted = sConfig.GetBoolDefault("RA.Stricted", false);
    iMinLevel = AccountTypes(sConfig.GetIntDefault("RA.MinLevel", SEC_ADMINISTRATOR));
    reference_counting_policy().value(ACE_Event_Handler::Reference_Counting_Policy::ENABLED);
}

RASocket::~RASocket()
{
    peer().close();
    sLog.outRALog("Connection was closed.");
}

// Accepts an incoming connection
int RASocket::open(void*)
{
    if (reactor()->register_handler(this, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::WRITE_MASK) == -1)
    {
        sLog.outError("RASocket::open: unable to register client handler errno = %s", ACE_OS::strerror(errno));
        return -1;
    }

    ACE_INET_Addr remote_addr;

    if (peer().get_remote_addr(remote_addr) == -1)
    {
        sLog.outError("RASocket::open: peer ().get_remote_addr errno = %s", ACE_OS::strerror(errno));
        return -1;
    }

    sLog.outRALog("Incoming connection from %s.", remote_addr.get_host_addr());

    // Print Motd
    sendf(sWorld.GetMotd());
    sendf("\r\n");
    sendf(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_USER));

    return 0;
}

int RASocket::close(int)
{
    if (closing_)
        return -1;

    DEBUG_LOG("RASocket::close");

    shutdown();
    closing_ = true;
    remove_reference();
    return 0;
}

int RASocket::handle_close(ACE_HANDLE h, ACE_Reactor_Mask)
{
    if (closing_)
        return -1;

    DEBUG_LOG("RASocket::handle_close");
    ACE_GUARD_RETURN(ACE_Thread_Mutex, Guard, outBufferLock, -1);

    closing_ = true;
    if (h == ACE_INVALID_HANDLE)
        peer().close_writer();
    remove_reference();
    return 0;
}

int RASocket::handle_output(ACE_HANDLE)
{
    ACE_GUARD_RETURN(ACE_Thread_Mutex, Guard, outBufferLock, -1);

    if (closing_)
        return -1;

    if (!outputBufferLen)
    {
        if (reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK) == -1)
        {
            sLog.outError("RASocket::handle_output: error while cancel_wakeup");
            return -1;
        }
        outActive = false;
        return 0;
    }

#ifdef MSG_NOSIGNAL
    ssize_t n = peer().send(outputBuffer, outputBufferLen, MSG_NOSIGNAL);
#else
    ssize_t n = peer().send(outputBuffer, outputBufferLen);
#endif

    if (n <= 0)
        return -1;

    ACE_OS::memmove(outputBuffer, outputBuffer + n, outputBufferLen - n);

    outputBufferLen -= n;

    return 0;
}

// Read data from the network
int RASocket::handle_input(ACE_HANDLE)
{
    DEBUG_LOG("RASocket::handle_input");
    if (closing_)
    {
        sLog.outError("Called RASocket::handle_input with closing_ = true");
        return -1;
    }

    size_t readBytes = peer().recv(inputBuffer + inputBufferLen, RA_BUFF_SIZE - inputBufferLen - 1);

    if (readBytes <= 0)
    {
        DEBUG_LOG("read " SIZEFMTD " bytes in RASocket::handle_input", readBytes);
        return -1;
    }

    // Discard data after line break or line feed
    bool gotenter = false;
    for (; readBytes > 0 ; --readBytes)
    {
        char c = inputBuffer[inputBufferLen];
        if (c == '\r' || c == '\n')
        {
            gotenter = true;
            break;
        }
        ++inputBufferLen;
    }

    if (gotenter)
    {
        inputBuffer[inputBufferLen] = 0;
        inputBufferLen = 0;
        switch (stage)
        {
            // If the input is '<username>'
            case NONE:
            {
                std::string szLogin = inputBuffer;

                accId = sAccountMgr.GetId(szLogin);

                // If the user is not found, deny access
                if (!accId)
                {
                    sendf("-No such user.\r\n");
                    sLog.outRALog("User %s does not exist.", szLogin.c_str());
                    if (bSecure)
                    {
                        handle_output();
                        return -1;
                    }
                    sendf("\r\n");
                    sendf(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_USER));
                    break;
                }

                accAccessLevel = sAccountMgr.GetSecurity(accId);

                // If gmlevel is too low: Deny access
                if (accAccessLevel < iMinLevel)
                {
                    sendf("-Not enough privileges.\r\n");
                    sLog.outRALog("User %s has no privilege.", szLogin.c_str());
                    if (bSecure)
                    {
                        handle_output();
                        return -1;
                    }
                    sendf("\r\n");
                    sendf(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_USER));
                    break;
                }

                // Allow by remotely connected admin use console level commands dependent from config setting
                if (accAccessLevel >= SEC_ADMINISTRATOR && !bStricted)
                    accAccessLevel = SEC_CONSOLE;

                stage = LG;
                sendf(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_PASS));
                break;
            }
            // If the input is '<password>' (and the user already gave his username)
            case LG:
            { 
                std::string pw = inputBuffer;

                // If Username and password ok
                if (sAccountMgr.CheckPassword(accId, pw))
                {
                    stage = OK;

                    sendf("+Logged in.\r\n");
                    sLog.outRALog("User account %u has logged in.", accId);
                    sendf("mangos>");
                }
                else
                {
                    // Else deny access
                    sendf("-Wrong pass.\r\n");
                    sLog.outRALog("User account %u has failed to log in.", accId);
                    if (bSecure)
                    {
                        handle_output();
                        return -1;
                    }
                    sendf("\r\n");
                    sendf(sObjectMgr.GetMangosStringForDBCLocale(LANG_RA_PASS));
                }
                break;
            }
            // If user is logged in: Parse and execute the command
            case OK:
                if (strlen(inputBuffer))
                {
                    sLog.outRALog("Got '%s' cmd.", inputBuffer);
                    if (strncmp(inputBuffer, "quit", 4) == 0)
                        return -1;
                    else
                    {
                        CliCommandHolder* cmd = new CliCommandHolder(accId, accAccessLevel, this, inputBuffer, &RASocket::zprint, &RASocket::commandFinished);
                        sWorld.QueueCliCommand(cmd);
                        pendingCommands.acquire();
                    }
                }
                else
                    sendf("mangos>");
                break;
                ///</ul>
        };
    }
    // No enter yet? wait for next input...
    return 0;
}

// Output function
void RASocket::zprint(void* callbackArg, const char* szText)
{
    if (!szText)
        return;

    ((RASocket*)callbackArg)->sendf(szText);
}

void RASocket::commandFinished(void* callbackArg, bool /*success*/)
{
    RASocket* raSocket = (RASocket*)callbackArg;
    raSocket->sendf("mangos>");
    raSocket->pendingCommands.release();
}

int RASocket::sendf(const char* msg)
{
    ACE_GUARD_RETURN(ACE_Thread_Mutex, Guard, outBufferLock, -1);

    if (closing_)
        return -1;

    int msgLen = strlen(msg);

    if (msgLen + outputBufferLen > RA_BUFF_SIZE)
        return -1;

    ACE_OS::memcpy(outputBuffer + outputBufferLen, msg, msgLen);
    outputBufferLen += msgLen;

    if (!outActive)
    {
        if (reactor()->schedule_wakeup
                (this, ACE_Event_Handler::WRITE_MASK) == -1)
        {
            sLog.outError("RASocket::sendf error while schedule_wakeup");
            return -1;
        }
        outActive = true;
    }
    return 0;
}
