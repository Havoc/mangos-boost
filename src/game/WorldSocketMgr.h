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

/** \addtogroup u2w User to World Communication
 *  @{
 *  \file WorldSocketMgr.h
 *  \author Derex <derex101@gmail.com>
 */

#ifndef __WORLDSOCKETMGR_H
#define __WORLDSOCKETMGR_H

#include <boost/thread/recursive_mutex.hpp>

#include <string>

#include "Policies/Singleton.h"
#include "Network/NetworkManager.h"


/// Manages all sockets connected to peers and network threads
class WorldSocketMgr : public NetworkManager, public MaNGOS::Singleton<WorldSocketMgr, MaNGOS::ClassLevelLockable<WorldSocketMgr, boost::recursive_mutex> > 
{
    public:
        friend class WorldSocket;
        friend class MaNGOS::OperatorNew<WorldSocketMgr>;

    private:
        virtual bool OnSocketOpen( const SocketPtr& sock ) override;

        virtual bool StartNetworkIO( boost::uint16_t port, const char* address ) override;

        WorldSocketMgr();
        virtual ~WorldSocketMgr();

        virtual SocketPtr CreateSocket( NetworkThread& owner ) override;

        int m_SockOutKBuff;
        int m_SockOutUBuff;
        bool m_UseNoDelay;
};

#define sWorldSocketMgr WorldSocketMgr::Instance()

#endif
/// @}
