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

#include "PatchHandler.h"
#include <ace/OS_NS_sys_socket.h>
#include <ace/OS_NS_dirent.h>
#include <ace/OS_NS_errno.h>
#include <ace/OS_NS_unistd.h>
#include <ace/os_include/netinet/os_tcp.h>
#include <boost/bind.hpp>
#include "Common.h"
#include "AuthCodes.h"
#include "Log.h"

INSTANTIATE_SINGLETON_1(PatchCache);

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
    uint8 cmd;
    uint16 data_size;
    uint8 data[4096]; // 4096 - page size on most arch
};

#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

PatchHandler::PatchHandler(protocol::Socket& socket, ACE_HANDLE patch) : socket_(socket), timer_(socket.get_io_service(),
    boost::posix_time::seconds(1)), patch_fd_(patch), send_buffer_(sizeof(Chunk))
{
    Chunk* data = (Chunk*)send_buffer_.read_data();
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
    Chunk* chunk = (Chunk*)send_buffer_.read_data();
    return sizeof(Chunk) - sizeof(chunk->data);
}

bool PatchHandler::open()
{
    if (patch_fd_ == ACE_INVALID_HANDLE)
        return false;

    timer_.async_wait(boost::bind(&PatchHandler::OnTimeout, shared_from_this(), boost::asio::placeholders::error));
    return true;
}

void PatchHandler::OnTimeout(const boost::system::error_code& error)
{
    if (error)
        return;

    TransmitFile();
}

void PatchHandler::TransmitFile()
{
    send_buffer_.Reset();

    Chunk* data = (Chunk*)send_buffer_.read_data();

    ssize_t r = ACE_OS::read(patch_fd_, data->data, sizeof(data->data));
    if (r <= 0)
        return;

    data->data_size = (uint16)r;
    send_buffer_.Commit(size_t(r) + offset());

    StartAsyncWrite();
}

void PatchHandler::StartAsyncWrite()
{
    socket_.async_write_some(boost::asio::buffer(send_buffer_.read_data(), send_buffer_.length()),
        boost::bind(&PatchHandler::OnWriteComplete, shared_from_this(),
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void PatchHandler::OnWriteComplete(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (error)
        return;

    send_buffer_.Consume(bytes_transferred);

    if (send_buffer_.length() > 0)
    {
        StartAsyncWrite();
        return;
    }

    TransmitFile();
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

    uint8 buf[check_chunk_size];

    while (!feof(pPatch))
    {
        size_t read = fread(buf, 1, check_chunk_size, pPatch);
        MD5_Update(&ctx, buf, read);
    }

    fclose(pPatch);

    // Store the result in the internal patch hash map
    patches_[path] = new PATCH_INFO;
    MD5_Final((uint8*) & patches_[path]->md5, &ctx);
}

bool PatchCache::GetHash(const char* pat, uint8 mymd5[MD5_DIGEST_LENGTH])
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