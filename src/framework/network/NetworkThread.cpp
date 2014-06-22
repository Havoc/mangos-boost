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

#include "NetworkThread.h"
#include "Database/DatabaseEnv.h"

NetworkThread::NetworkThread() : connections_(0)
{

}

NetworkThread::~NetworkThread()
{
    Stop();
}

void NetworkThread::Start()
{
    service_work_.reset(new protocol::Service::work(service_));
    thread_.reset(new boost::thread(boost::bind(&NetworkThread::Work, this)));
}

void NetworkThread::Stop()
{
    service_work_.reset();
    service_.stop();

    if (thread_.get())
    {
        thread_->join();
        thread_.reset();
    }
}

void NetworkThread::AddSocket(const SocketPtr& socket)
{
    ++connections_;
    boost::lock_guard<boost::mutex> lock(mutex_);
    sockets_.insert(socket);
}

void NetworkThread::RemoveSocket(const SocketPtr& socket)
{
    --connections_;
    boost::lock_guard<boost::mutex> lock(mutex_);
    sockets_.erase(socket);
}

void NetworkThread::Work()
{
    DEBUG_LOG("Network Thread Starting");
    LoginDatabase.ThreadStart();
    service_.run();
    LoginDatabase.ThreadEnd();
    DEBUG_LOG("Network Thread Exitting");
}