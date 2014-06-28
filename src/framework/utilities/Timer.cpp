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

#include <chrono>
#include "Timer.h"

uint32 WorldTimer::m_iTime = 0;
uint32 WorldTimer::m_iPrevTime = 0;

uint32 WorldTimer::tick()
{
    // save previous world tick time
    m_iPrevTime = m_iTime;

    // get the new one and don't forget to persist current system time in m_SystemTickTime
    m_iTime = WorldTimer::getMSTime();

    // return tick diff
    return getMSTimeDiff(m_iPrevTime, m_iTime);
}

uint32 WorldTimer::getMSTime()
{
    static const std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
    return uint32((std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now() - start_time).count()) % UI64LIT(0x00000000FFFFFFFF));
}
