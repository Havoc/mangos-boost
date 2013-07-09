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

#include "NetworkBuffer.h"

NetworkBuffer::NetworkBuffer() : write_position_(0), read_position_(0),
    size_(0), data_allocated_(false)
{

}

NetworkBuffer::NetworkBuffer(const uint32 size) : write_position_(0), read_position_(0),
    size_(size), data_allocated_(true)
{
    data_ = new uint8[size];
}

NetworkBuffer::NetworkBuffer(uint8* buffer, const uint32 size) : write_position_(0), read_position_(0),
    size_(size), data_allocated_(false)
{
    data_ = buffer;
}

NetworkBuffer::~NetworkBuffer()
{
    Deallocate();
}

void NetworkBuffer::Allocate(const uint32 size)
{
    if (data_ == nullptr)
    {
        size_ = size;
        data_ = new uint8[size];
        data_allocated_ = true;
    }
}

void NetworkBuffer::Reallocate(const uint32 new_size)
{
    if (data_allocated_)
    {
        delete [] data_;
        size_ = new_size;
        data_ = new uint8[new_size];
        Reset();
    }
}

void NetworkBuffer::Deallocate()
{
    if (data_allocated_)
    {
        delete [] data_;
        data_allocated_ = false;
    }
}

void NetworkBuffer::AssignBuffer(uint8* buffer, const uint32 size)
{
    Deallocate();
    size_ = size;
    data_ = buffer;
}

void NetworkBuffer::UnassignBuffer()
{
    if (!data_allocated_)
    {
        size_ = 0;
        data_ = nullptr;
        Reset();
    }
}

bool NetworkBuffer::Write(const uint8* data, const size_t n)
{
    if (data_ == nullptr || data == nullptr || n > space())
        return false;

    std::memcpy(&data_[write_position_], data, n);
    Commit(n);
    return true;
}

bool NetworkBuffer::Read(uint8* data, const size_t n)
{
    if (data_ == nullptr || data == nullptr || n > length())
        return false;

    std::memcpy(data, &data_[read_position_], n);
    Consume(n);
    return true;
}

bool NetworkBuffer::ReadNoConsume(uint8* data, const size_t n)
{
    if (data_ == nullptr || data == nullptr || n > length())
        return false;

    std::memcpy(data, &data_[read_position_], n);
    return true;
}

void NetworkBuffer::Commit(const size_t n)
{
    uint32 pos = write_position_ + n;
    if (capacity() >= pos)
        write_position_ = pos;
}

void NetworkBuffer::Consume(const size_t n)
{
    uint32 pos = read_position_ + n;
    if (capacity() >= pos)
        read_position_ = pos;
}

void NetworkBuffer::Prepare()
{
    if (!Crunch())
        Reset();
}

bool NetworkBuffer::Crunch()
{
    if (data_ != nullptr && length() != 0)
    {
        if (read_position_ > write_position_)
            return false;

        size_t len = length();
        std::memmove(data_, &data_[read_position_], len);
        write_position_ = len;
        read_position_ = 0;
        return true;
    }
    return false;
}

void NetworkBuffer::Reset()
{
    write_position_ = read_position_ = 0;
}

uint8* NetworkBuffer::read_data() const
{
    if (data_ == nullptr)
        return nullptr;

    return &data_[read_position_];
}

uint8* NetworkBuffer::write_data() const
{
    if (data_ == nullptr)
        return nullptr;

    return &data_[write_position_];
}

const uint32 NetworkBuffer::length() const
{
    return write_position_ - read_position_;
}

const uint32 NetworkBuffer::space() const
{
    return size_ - write_position_;
}

const uint32 NetworkBuffer::capacity() const
{
    return size_;
}