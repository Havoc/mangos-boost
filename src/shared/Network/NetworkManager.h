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

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <boost/scoped_array.hpp>

#include "ProtocolDefinitions.h"

class NetworkThread;

/// Manages all sockets connected to peers and network threads
class NetworkManager
{
public:
    friend class Socket;

    /// Start network, listen at address:port .
    bool StartNetwork( boost::uint16_t port, std::string& address );

    /// Stops all network threads, It will wait for all running threads .
    void StopNetwork();

    /// Wait until all network threads have "joined" .
    void Wait();

    const std::string& GetBindAddress() { return m_addr; }

    boost::uint16_t GetBindPort() { return m_port; }

protected:

    NetworkManager();
    virtual ~NetworkManager();

    virtual bool StartNetworkIO( boost::uint16_t port, const char* address );

    virtual SocketPtr CreateSocket( NetworkThread& owner ) = 0;

    virtual bool OnSocketOpen( const SocketPtr& sock );

    virtual void OnSocketClose( const SocketPtr& sock);

    size_t m_NetThreadsCount;

private:

    void accept_next_connection();

    NetworkThread& get_acceptor_thread();

    NetworkThread& get_network_thread_for_new_connection();

    void OnNewConnection( SocketPtr connection,
        const boost::system::error_code& error );

    void Stop();

    std::string m_addr;
    boost::uint16_t m_port;

    std::auto_ptr<protocol::Acceptor> m_acceptor;
    boost::scoped_array<NetworkThread> m_NetThreads;

    bool m_isRunning;
};

#endif