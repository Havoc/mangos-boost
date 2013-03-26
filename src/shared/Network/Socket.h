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

#ifndef SOCKET_H
#define SOCKET_H

#include "ProtocolDefinitions.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "Common.h"
#include "Auth/AuthCrypt.h"
#include "Auth/BigNumber.h"

class NetworkThread;
class NetworkManager;

class Socket : public boost::enable_shared_from_this<Socket>
{

public:
    /// Declare some friends
    friend class NetworkManager;

    Socket( NetworkManager& socketMrg, 
            NetworkThread& owner );

    virtual ~Socket(void);

    /// Check if socket is closed.
    bool IsClosed(void) const;

    /// Close the socket.
    virtual void CloseSocket(void);

    /// Get address of connected peer.
    const std::string& GetRemoteAddress(void) const;

    /// Enable TcpNoDelay
    bool EnableTCPNoDelay( bool enable );

    /// Set SO_SDNBUF variable 
    bool SetSendBufferSize( int size );

    /// Set custom value for outgoing buffer size
    void SetOutgoingBufferSize( size_t size );

    /// Get underlying socket object
    protocol::Socket& socket() { return m_socket; }

protected:

    /// Called on open ,the void* is the acceptor.
    virtual bool open();

    uint32 native_handle();

    /// Schedule asynchronous send operation
    void start_async_send();

    virtual bool process_incoming_data() = 0;

    /// Mutex type used for various synchronizations.
    typedef boost::mutex LockType;
    typedef boost::lock_guard<LockType> GuardType;

    /// Mutex for protecting output related data.
    LockType m_OutBufferLock;

    /// Buffer used for writing output.
    std::auto_ptr<NetworkBuffer> m_OutBuffer;

    /// Buffer used for receiving input
    std::auto_ptr<NetworkBuffer> m_ReadBuffer;

private:

    void on_write_complete( const boost::system::error_code& error, 
        size_t bytes_transferred );

    void start_async_read();

    void on_read_complete( const boost::system::error_code& error, 
        size_t bytes_transferred );

    void reset( NetworkBuffer& buffer );

    void OnError( const boost::system::error_code& error );

    std::string obtain_remote_address() const;

    void close();

    NetworkThread& owner() { return m_owner; }

    protocol::Socket m_socket;

    NetworkManager& m_manager;

    NetworkThread& m_owner;

    /// Address of the remote peer
    std::string m_Address;

    /// Size of the m_OutBuffer.
    size_t m_OutBufferSize;

    /// True if the socket has an outstanding write operation
    bool m_OutActive;

    /// True is socket is closed
    bool m_closing;

    static const std::string UNKNOWN_NETWORK_ADDRESS;;
};

#endif