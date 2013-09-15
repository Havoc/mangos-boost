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

#ifndef NETWORK_THREAD_H
#define NETWORK_THREAD_H

#include <set>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "ProtocolDefinitions.h"

class NetworkThread : public boost::noncopyable
{
public:
    NetworkThread();

    virtual ~NetworkThread();

    void Start();
    void Stop();

    void AddSocket(const SocketPtr& socket);
    void RemoveSocket(const SocketPtr& socket);

    long Connections() const { return connections_; }
    protocol::Service& service() { return service_; }

private:
    virtual void Work();

    typedef std::set<SocketPtr> SocketSet;
    SocketSet sockets_;

    boost::atomic_long connections_;

    protocol::Service service_;
    std::auto_ptr<protocol::Service::work> service_work_;

    std::auto_ptr<boost::thread> thread_;
    boost::mutex mutex_;
};

#endif // NETWORK_THREAD_H