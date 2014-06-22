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

#include "NetworkManager.h"
#include <boost/bind.hpp>
#include "Common.h"
#include "NetworkThread.h"
#include "Socket.h"
#include "Log.h"

NetworkManager::NetworkManager() : network_threads_count_(1), running_(false)
{

}

NetworkManager::~NetworkManager()
{
    StopNetwork();
    acceptor_.reset();
    network_threads_.reset();
}

bool NetworkManager::StartNetwork(boost::uint16_t port, std::string address)
{
    if (running_)
        return false;

    address_ = address;
    port_ = port;

    if (network_threads_count_ <= 0)
    {
        sLog.outError("Number of network threads is incorrect = %i", network_threads_count_);
        return false;
    }

    network_threads_count_ += 1;
    network_threads_.reset(new NetworkThread[network_threads_count_]);

    try
    {
        protocol::Endpoint listen_address(protocol::IPAddress::from_string(address_), port_);
        acceptor_.reset(new protocol::Acceptor(get_acceptor_thread().service(), listen_address));
    }
    catch (boost::system::error_code&)
    {
        sLog.outError("Failed to open acceptor, check if the port is free");
        return false;
    }

    running_ = true;

    AcceptNewConnection();

    for (size_t i = 0; i < network_threads_count_; ++i)
        network_threads_[i].Start();
    
    return true;
}

void NetworkManager::StopNetwork()
{
    if (running_)
    {
        if (acceptor_.get())
            acceptor_->cancel();

        if (network_threads_)
            for (size_t i = 0; i < network_threads_count_; ++i)
                network_threads_[i].Stop();

        running_ = false;
    }
}

bool NetworkManager::OnSocketOpen(const SocketPtr& socket)
{
    NetworkThread& thread = socket->owner();
    thread.AddSocket(socket);

    return true;
}

void NetworkManager::OnSocketClose(const SocketPtr& socket)
{
    NetworkThread& thread = socket->owner();
    thread.RemoveSocket(socket);
}

void NetworkManager::AcceptNewConnection()
{
    NetworkThread& worker = get_network_thread_for_new_connection();
    SocketPtr connection = CreateSocket(worker);

    acceptor_->async_accept(connection->socket(),
        boost::bind(&NetworkManager::OnNewConnection, this, connection, boost::asio::placeholders::error));
}

void NetworkManager::OnNewConnection(SocketPtr connection, const boost::system::error_code& error)
{
    if (error)
    {
        sLog.outError("Error accepting new client connection!");
        return;
    }
    
    if (!connection->Open())
    {
        sLog.outError("Unable to start new client connection!");

        connection->CloseSocket();
        return;
    }

    AcceptNewConnection();
}

NetworkThread& NetworkManager::get_acceptor_thread()
{
    return network_threads_[0];
}

NetworkThread& NetworkManager::get_network_thread_for_new_connection()
{
    // Skip acceptor thread
    size_t min = 1;

    MANGOS_ASSERT(network_threads_count_ > 1);

    for (size_t i = 1; i < network_threads_count_; ++i)
    {
        if (network_threads_[i].Connections() < network_threads_[min].Connections())
            min = i;
    }

    return network_threads_[min];
}