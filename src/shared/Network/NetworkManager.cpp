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

#include "Common.h"
#include "NetworkManager.h"
#include "NetworkThread.h"
#include "Socket.h"
#include "Log.h"

#include <boost/bind.hpp>

NetworkManager::NetworkManager():
    m_NetThreadsCount(1),
    m_isRunning(false)
{

}

NetworkManager::~NetworkManager()
{
    StopNetwork();

    m_acceptor.reset();
    m_NetThreads.reset();
}

bool NetworkManager::StartNetworkIO( boost::uint16_t port, const char* address )
{
    if( m_NetThreadsCount <= 0 )
    {
        sLog.outError("Number of network threads is incorrect = %i", m_NetThreadsCount );
        return false;
    }

    m_NetThreadsCount += 1;
    m_NetThreads.reset( new NetworkThread[ m_NetThreadsCount ] );

    try
    {
        protocol::Endpoint listen_addr( protocol::IPAddress::from_string( address ), port );
        m_acceptor.reset( new protocol::Acceptor( get_acceptor_thread().service() , listen_addr ) );
    }
    catch( boost::system::error_code&  )
    {
        sLog.outError("Failed to open acceptor, check if the port is free");
        return false;
    }

    m_isRunning = true;

    accept_next_connection();

    for (size_t i = 0; i < m_NetThreadsCount; ++i)
        m_NetThreads[i].Start();
    
    return true;
}

bool NetworkManager::StartNetwork( boost::uint16_t port, std::string& address)
{
    m_addr = address;
    m_port = port;

    return StartNetworkIO(port, address.c_str());
}

void NetworkManager::StopNetwork()
{
    if( m_isRunning )
    {
        m_isRunning = false;

        Stop();
        Wait();
    }
}

void NetworkManager::Wait()
{
    if( m_NetThreads )
    {
        for (size_t i = 0; i < m_NetThreadsCount; ++i)
            m_NetThreads[i].Wait();
    }
}

void NetworkManager::Stop()
{
    if( m_acceptor.get() )
        m_acceptor->cancel();

    if( m_NetThreads )
    {
        for (size_t i = 0; i < m_NetThreadsCount; ++i)
            m_NetThreads[i].Stop();
    }
}

bool NetworkManager::OnSocketOpen( const SocketPtr& sock )
{
    NetworkThread& thrd = sock->owner();
    thrd.AddSocket( sock );

    return true;
}

void NetworkManager::OnSocketClose( const SocketPtr& sock )
{
    NetworkThread& thrd = sock->owner();
    thrd.RemoveSocket( sock );
}

void NetworkManager::accept_next_connection()
{
    NetworkThread& worker = get_network_thread_for_new_connection();
    SocketPtr connection = CreateSocket( worker );

    m_acceptor->async_accept( connection->socket(), 
                              boost::bind( &NetworkManager::OnNewConnection, this, connection, 
                                           boost::asio::placeholders::error) );
}

void NetworkManager::OnNewConnection( SocketPtr connection, 
                                      const boost::system::error_code& error )
{
    if( error )
    {
        sLog.outError("Error accepting new client connection!");
        return;
    }
    
    if( !connection->open() )
    {
        sLog.outError("Unable to start new client connection!");

        connection->CloseSocket();
        return;
    }

    accept_next_connection();
}

NetworkThread& NetworkManager::get_acceptor_thread()
{
    return m_NetThreads[0];
}

NetworkThread& NetworkManager::get_network_thread_for_new_connection()
{
    //we skip the Acceptor Thread
    size_t min = 1;

    MANGOS_ASSERT(m_NetThreadsCount > 1);

    for (size_t i = 1; i < m_NetThreadsCount; ++i)
    {
        if (m_NetThreads[i].Connections() < m_NetThreads[min].Connections())
            min = i;
    }

    return m_NetThreads[min];
}
