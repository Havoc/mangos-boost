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

#include <boost/bind.hpp>

#include "Socket.h"
#include "Common.h"

#include "Util.h"
#include "Auth/Sha1.h"
#include "Log.h"
#include "NetworkThread.h"
#include "NetworkManager.h"

const std::string Socket::UNKNOWN_NETWORK_ADDRESS = "<unknown>";

Socket::Socket( NetworkManager& socketMrg, 
                NetworkThread& owner ) :
    m_manager(socketMrg),
    m_owner(owner),
    m_socket(owner.service()),
    m_OutBufferSize(protocol::SEND_BUFFER_SIZE),
    m_OutActive(false),
    m_closing(true),
    m_Address(UNKNOWN_NETWORK_ADDRESS)
{

}

Socket::~Socket(void)
{
    close();
}

bool Socket::IsClosed(void) const
{
    return m_closing;
}

void Socket::CloseSocket(void)
{
    if( IsClosed() )
        return;

    close();

    m_manager.OnSocketClose( shared_from_this() );
}

const std::string& Socket::GetRemoteAddress(void) const
{
    return m_Address;
}

bool Socket::open()
{
    // Prevent double call to this func.
    if ( m_OutBuffer.get() )
        return false;

    // Store peer address.
    m_Address = obtain_remote_address();
    if( m_Address == UNKNOWN_NETWORK_ADDRESS )
        return false;

    // Hook for the manager.
    if ( !m_manager.OnSocketOpen( shared_from_this() ) )
        return false;

    m_closing = false;

    // Allocate buffers.
    m_OutBuffer.reset( new NetworkBuffer(m_OutBufferSize) );
    m_ReadBuffer.reset( new NetworkBuffer( protocol::READ_BUFFER_SIZE ) );

    // Start reading data from client
    start_async_read();

    return true;
}

void Socket::close()
{
    if( IsClosed() )
        return;

    m_closing = true;

    try
    {
        if( m_socket.is_open() )
        {
            m_socket.shutdown( boost::asio::socket_base::shutdown_both );
            m_socket.close();
        }
    }
    catch( boost::system::error_code& e)
    {
        sLog.outError("Socket::close: error occurred while closing socket = %s", e.message().c_str());
    }
}

bool Socket::EnableTCPNoDelay( bool enable )
{
    try
    {
        m_socket.set_option( boost::asio::ip::tcp::no_delay(enable) );
    }
    catch( boost::system::error_code& error )
    {
        sLog.outError("Socket::SetTCPNoDelay: set_option TCP_NODELAY errno = %s", error.message().c_str());
        return false;
    }

    return true;
}

bool Socket::SetSendBufferSize( int size )
{
    try
    {
        m_socket.set_option( boost::asio::socket_base::send_buffer_size(size) );
    }
    catch( boost::system::error_code& error )
    {
        sLog.outError("Socket::SetSendBufferSize set_option SO_SNDBUF with size %i errno = %s", size, error.message().c_str());
        return false;
    }

    return true;
}

void Socket::SetOutgoingBufferSize( size_t size )
{
    m_OutBufferSize = size;
}

uint32 Socket::native_handle() 
{
    return uint32( m_socket.native_handle() );
}

void Socket::start_async_send()
{
    if( IsClosed() )
        return;

    if( m_OutActive )
        return;
    
    if ( m_OutBuffer->length() == 0 )
    {
        m_OutActive = false;
        return;
    }

    m_OutActive = true;

    m_socket.async_write_some( boost::asio::buffer( m_OutBuffer->rd_ptr(), m_OutBuffer->length() ),
                               boost::bind( &Socket::on_write_complete, shared_from_this(), 
                                            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
}

void Socket::on_write_complete( const boost::system::error_code& error,
                                     size_t bytes_transferred )
{
    if( error )
    {
        OnError( error );
        return;
    }

    GuardType Lock(m_OutBufferLock);

    m_OutActive = false;

    m_OutBuffer->rd_ptr( bytes_transferred );

    reset( *m_OutBuffer ); 

    start_async_send();
}

void Socket::start_async_read()
{
    if( IsClosed() )
        return;

    reset( *m_ReadBuffer );

    m_socket.async_read_some( boost::asio::buffer( m_ReadBuffer->wr_ptr(), m_ReadBuffer->space() ),
                              boost::bind( &Socket::on_read_complete, shared_from_this(), 
                                           boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
}

void Socket::on_read_complete( const boost::system::error_code& error,
                                    size_t bytes_transferred )
{
    if( error )
    {
        OnError( error );
        return;
    }

    if( bytes_transferred > 0 )
    {
        m_ReadBuffer->wr_ptr( bytes_transferred );

        if( !process_incoming_data() )
        {
            CloseSocket();
            return;
        }
    }

    start_async_read();
}

void Socket::reset( NetworkBuffer& buffer )
{
    if( buffer.length() == 0 )
    {
        buffer.reset();
    }
    else
    {
        buffer.crunch();
    }
}

void Socket::OnError( const boost::system::error_code& error )
{
    if( !error )
        return;

    CloseSocket();

    //don't log EOF errors since they notify about remote client connection close
    if( error != boost::asio::error::eof )
        sLog.outError("Network error occurred = %s. Closing connection", error.message().c_str());
}

std::string Socket::obtain_remote_address() const
{
    try
    {
        protocol::Endpoint remote_addr = m_socket.remote_endpoint();
        protocol::IPAddress ip_addr = remote_addr.address();
        return ip_addr.to_string();
    }
    catch( boost::system::error_code& error )
    {
        sLog.outError("Socket::obtain_remote_address: errno = %s",error.message().c_str());
    }

    return UNKNOWN_NETWORK_ADDRESS;
}