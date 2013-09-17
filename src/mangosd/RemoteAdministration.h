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

#ifndef REMOTE_ADMINISTRATION_H
#define REMOTE_ADMINISTRATION_H

#include <ace/Synch_Traits.h>
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Acceptor.h>
#include <ace/Thread_Mutex.h>
#include <ace/Semaphore.h>
#include <ace/OS_NS_signal.h>
#include <ace/TP_Reactor.h>
#include <ace/Dev_Poll_Reactor.h>
#include "Common.h"
#include "Config/Config.h"
#include "Threading.h"
#include "Log.h"
#include "World.h"

#define RA_BUFF_SIZE 8192

/// Remote Administration Socket
typedef ACE_Svc_Handler < ACE_SOCK_STREAM, ACE_NULL_SYNCH> RAHandler;
class RASocket: protected RAHandler
{
public:
    ACE_Semaphore pendingCommands;
    typedef ACE_Acceptor<RASocket, ACE_SOCK_ACCEPTOR > Acceptor;
    friend class ACE_Acceptor<RASocket, ACE_SOCK_ACCEPTOR >;

    int sendf(const char*);

protected:
    // Things called by ACE framework.
    RASocket(void);
    virtual ~RASocket(void);

    // Called on open ,the void* is the acceptor.
    virtual int open(void*) override;

    // Called on failures inside of the acceptor, don't call from your code.
    virtual int close(int);

    // Called when we can read from the socket.
    virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE) override;

    // Called when the socket can write.
    virtual int handle_output(ACE_HANDLE = ACE_INVALID_HANDLE) override;

    // Called when connection is closed or error happens.
    virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE, ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

private:
    bool outActive;

    char inputBuffer[RA_BUFF_SIZE];
    uint32 inputBufferLen;

    ACE_Thread_Mutex outBufferLock;
    char outputBuffer[RA_BUFF_SIZE];
    uint32 outputBufferLen;

    uint32 accId;
    AccountTypes accAccessLevel;
    bool bSecure;                                       // Kick on wrong pass, non exist. user OR user with no priv
    // Will protect from DOS, bruteforce attacks
    bool bStricted;                                     // Not allow execute console only commands (SEC_CONSOLE) remotly
    AccountTypes iMinLevel;
    enum
    {
        NONE, // Initial value
        LG,   // At login
        OK,   // Login successful and user has enough privileges
    } stage;

    static void zprint(void* callbackArg, const char* szText);
    static void commandFinished(void* callbackArg, bool success);
};

/// Thread for handling the remote administration service
class RARunnable : public MaNGOS::Runnable
{
public:
    RARunnable()
    {
        ACE_Reactor_Impl* imp = 0;

#if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
        imp = new ACE_Dev_Poll_Reactor();
        imp->max_notify_iterations(128);
        imp->restart(1);
#else
        imp = new ACE_TP_Reactor();
        imp->max_notify_iterations(128);
#endif
        m_Reactor = new ACE_Reactor(imp, 1 /* 1= delete implementation so we don't have to care */);
        m_Acceptor = new RASocket::Acceptor;
    }

    ~RARunnable()
    {
        delete m_Reactor;
        delete m_Acceptor;
    }

    void run()
    {
        uint16 raport = sConfig.GetIntDefault("Ra.Port", 3443);
        std::string stringip = sConfig.GetStringDefault("Ra.IP", "0.0.0.0");

        ACE_INET_Addr listen_addr(raport, stringip.c_str());

        if (m_Acceptor->open(listen_addr, m_Reactor, ACE_NONBLOCK) == -1)
            sLog.outError("MaNGOS RA can not bind to port %d on %s", raport, stringip.c_str());

        sLog.outString("Starting Remote access listner on port %d on %s", raport, stringip.c_str());

        while (!m_Reactor->reactor_event_loop_done())
        {
            ACE_Time_Value interval(0, 10000);

            if (m_Reactor->run_reactor_event_loop(interval) == -1)
                break;

            if (World::IsStopped())
            {
                m_Acceptor->close();
                break;
            }
        }
        sLog.outString("RARunnable thread ended");
    }

private:
    ACE_Reactor* m_Reactor;
    RASocket::Acceptor* m_Acceptor;
};

#endif // REMOTE_ADMINISTRATION_H