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

#ifndef WORLD_SOCKET_H
#define WORLD_SOCKET_H

#include <ace/Time_Value.h>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include "Common.h"
#include "AuthCrypt.h"
#include "BigNumber.h"
#include "Socket.h"
#include "ByteConverter.h"
#include "Log.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct ServerPktHeader
{
    /**
    * size is the length of the payload _plus_ the length of the opcode
    */
    ServerPktHeader(uint32 size, uint16 cmd) : size(size)
    {
        uint8 headerIndex = 0;
        if (isLargePacket())
        {
            DEBUG_LOG("initializing large server to client packet. Size: %u, cmd: %u", size, cmd);
            header[headerIndex++] = 0x80 | (0xFF & (size >> 16));
        }
        header[headerIndex++] = 0xFF & (size >> 8);
        header[headerIndex++] = 0xFF & size;

        header[headerIndex++] = 0xFF & cmd;
        header[headerIndex++] = 0xFF & (cmd >> 8);
    }

    uint8 getHeaderLength()
    {
        // cmd = 2 bytes, size= 2||3bytes
        return 2 + (isLargePacket() ? 3 : 2);
    }

    bool isLargePacket()
    {
        return size > 0x7FFF;
    }

    const uint32 size;
    uint8 header[5];
};

struct ClientPktHeader
{
    bool IsValid() const
    {
        return (size >= 4) && (size <= 10240) && (cmd <= 10240);
    }

    void Convert()
    {
        EndianConvertReverse(size);
        EndianConvert(cmd);
    }

    uint16 size;
    uint32 cmd;
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

class WorldPacket;
class WorldSession;
class NetworkThread;
class WorldSocketMgr;

/**
 * WorldSocket.
 *
 * This class is responsible for the communication with
 * remote clients.
 * Most methods return false on failure.
 * The class uses reference counting.
 *
 * For output the class uses one buffer (64K usually) and
 * a queue where it stores packet if there is no place on
 * the queue. The reason this is done, is because the server
 * does really a lot of small-size writes to it, and it doesn't
 * scale well to allocate memory for every. When something is
 * written to the output buffer the socket is not immediately
 * activated for output (again for the same reason), there
 * is 10ms celling (thats why there is Update() override method).
 * This concept is similar to TCP_CORK, but TCP_CORK
 * uses 200ms celling. As result overhead generated by
 * sending packets from "producer" threads is minimal,
 * and doing a lot of writes with small size is tolerated.
 *
 * The calls to Update () method are managed by WorldSocketMgr
 * and ReactorRunnable.
 *
 * For input ,the class uses one 1024 bytes buffer on stack
 * to which it does recv() calls. And then received data is
 * distributed where its needed. 1024 matches pretty well the
 * traffic generated by client for now.
 *
 * The input/output do speculative reads/writes (AKA it tryes
 * to read all data available in the kernel buffer or tryes to
 * write everything available in userspace buffer),
 * which is ok for using with Level and Edge Triggered IO
 * notification.
 *
 */

class WorldSocket : public Socket
{
public:
    const static int CLIENT_PACKET_HEADER_SIZE = sizeof(ClientPktHeader);

    WorldSocket(NetworkManager& manager, NetworkThread& owner);
    virtual ~WorldSocket(void);

    virtual void CloseSocket(void) override;
    bool SendPacket(const WorldPacket& pct);
    BigNumber& GetSessionKey() { return session_key_; }

protected:
    virtual bool Open() override;
    virtual bool ProcessIncomingData() override;

private:
    bool ReadPacketHeader();
    bool ValidatePacketHeader();
    bool ReadPacketContent();

    bool ProcessPacket(WorldPacket* new_pct);

    bool HandleAuthSession(WorldPacket& recvPacket);
    bool HandlePing(WorldPacket& recvPacket);

    bool received_header_;

    // Client packet
    ClientPktHeader header_;
    WorldPacket* packet_;

    // Used for de-/encrypting packet headers
    AuthCrypt crypt_;

    uint32 seed_;
    BigNumber session_key_;

    // Session to which received packets are routed
    WorldSession* session_;

    // Mutex lock to protect session_
    LockType session_lock_;

    // Time in which the last ping was received
    ACE_Time_Value m_LastPingTime;

    // Keep track of over-speed pings ,to prevent ping flood.
    uint32 m_OverSpeedPings;
};

typedef boost::shared_ptr<WorldSocket> WorldSocketPtr;

#endif // WORLD_SOCKET_H