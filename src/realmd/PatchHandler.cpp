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

/** \file
  \ingroup realmd
  */

#include "Common.h"
#include "PatchHandler.h"
#include "AuthCodes.h"
#include "Log.h"

#include <ace/OS_NS_sys_socket.h>
#include <ace/OS_NS_dirent.h>
#include <ace/OS_NS_errno.h>
#include <ace/OS_NS_unistd.h>

#include <ace/os_include/netinet/os_tcp.h>

#include <boost/bind.hpp>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct Chunk
{
    ACE_UINT8 cmd;
    ACE_UINT16 data_size;
    ACE_UINT8 data[4096]; // 4096 - page size on most arch
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

PatchHandler::PatchHandler(protocol::Socket& socket, ACE_HANDLE patch) :
    m_socket( socket ),
    m_timer( m_socket.get_io_service(), boost::posix_time::seconds(1) ),
    patch_fd_( patch ),
    m_sendBuffer( sizeof(Chunk) )
{
    Chunk * data = (Chunk *)m_sendBuffer.rd_ptr();
    data->cmd = CMD_XFER_DATA;
    data->data_size = 0;
}

PatchHandler::~PatchHandler()
{
    if (patch_fd_ != ACE_INVALID_HANDLE)
        ACE_OS::close(patch_fd_);
}

size_t PatchHandler::offset() const
{
    Chunk * chunk = (Chunk *)m_sendBuffer.rd_ptr();
    return sizeof(Chunk) - sizeof(chunk->data);
}

bool PatchHandler::open()
{
    if ( patch_fd_ == ACE_INVALID_HANDLE)
        return false;

    m_timer.async_wait( boost::bind(&PatchHandler::on_timeout, shared_from_this(), boost::asio::placeholders::error) );
    return true;
}

void PatchHandler::on_timeout( const boost::system::error_code& error)
{
    if( error )
        return;

    transmit_file();
}


void PatchHandler::transmit_file()
{
    m_sendBuffer.reset();

    Chunk * data = (Chunk *)m_sendBuffer.rd_ptr();

    ssize_t r = ACE_OS::read(patch_fd_, data->data, sizeof(data->data));
    if( r <= 0 )
        return;

    data->data_size = (ACE_UINT16)r;
    m_sendBuffer.wr_ptr( size_t(r) + offset() );

    start_async_write();
}

void PatchHandler::start_async_write()
{
    m_socket.async_write_some( boost::asio::buffer( m_sendBuffer.rd_ptr(), m_sendBuffer.length() ),
        boost::bind( &PatchHandler::on_write_complete, shared_from_this(), 
                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ));
}

void PatchHandler::on_write_complete( const boost::system::error_code& error, 
                                      size_t bytes_transferred )
{
    if( error )
        return;

    m_sendBuffer.rd_ptr( bytes_transferred );

    if(m_sendBuffer.length() > 0 )
    {
        start_async_write();
        return;
    }

    transmit_file();
}

PatchCache::~PatchCache()
{
    for (Patches::iterator i = patches_.begin(); i != patches_.end(); ++i)
        delete i->second;
}

PatchCache::PatchCache()
{
    LoadPatchesInfo();
}

PatchCache* PatchCache::instance()
{
    return ACE_Singleton<PatchCache, ACE_Thread_Mutex>::instance();
}

void PatchCache::LoadPatchMD5(const char* szFileName)
{
    // Try to open the patch file
    std::string path = "./patches/";
    path += szFileName;
    FILE* pPatch = fopen(path.c_str(), "rb");
    sLog.outDebug("Loading patch info from %s", path.c_str());

    if (!pPatch)
        return;

    // Calculate the MD5 hash
    MD5_CTX ctx;
    MD5_Init(&ctx);

    const size_t check_chunk_size = 4 * 1024;

    ACE_UINT8 buf[check_chunk_size];

    while (!feof(pPatch))
    {
        size_t read = fread(buf, 1, check_chunk_size, pPatch);
        MD5_Update(&ctx, buf, read);
    }

    fclose(pPatch);

    // Store the result in the internal patch hash map
    patches_[path] = new PATCH_INFO;
    MD5_Final((ACE_UINT8*) & patches_[path]->md5, &ctx);
}

bool PatchCache::GetHash(const char* pat, ACE_UINT8 mymd5[MD5_DIGEST_LENGTH])
{
    for (Patches::iterator i = patches_.begin(); i != patches_.end(); ++i)
        if (!stricmp(pat, i->first.c_str()))
        {
            memcpy(mymd5, i->second->md5, MD5_DIGEST_LENGTH);
            return true;
        }

    return false;
}

void PatchCache::LoadPatchesInfo()
{
    ACE_DIR* dirp = ACE_OS::opendir(ACE_TEXT("./patches/"));

    if (!dirp)
        return;

    ACE_DIRENT* dp;

    while ((dp = ACE_OS::readdir(dirp)) != NULL)
    {
        int l = strlen(dp->d_name);
        if (l < 8)
            continue;

        if (!memcmp(&dp->d_name[l - 4], ".mpq", 4))
            LoadPatchMD5(dp->d_name);
    }

    ACE_OS::closedir(dirp);
}

