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

#ifndef PROCESS_PRIORITY_H_
#define PROCESS_PRIORITY_H_

#include "Common.h"
#include "Log.h"

#ifdef WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <Windows.h>
#endif

namespace cmangos {
namespace helper {

void SetProcessPriority(uint32 affinity, bool high_priority)
{
#ifdef WIN32
    HANDLE hProcess = GetCurrentProcess();
    if (affinity > 0)
    {
        ULONG_PTR application_affinity;
        ULONG_PTR system_affinity;

        if (GetProcessAffinityMask(hProcess, &application_affinity, &system_affinity))
        {
            // Remove non accessible processors
            ULONG_PTR current_affinity = affinity & application_affinity;

            if (!current_affinity)
                sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for realmd. Accessible processors bitmask (hex): %x", affinity, application_affinity);
            else if (SetProcessAffinityMask(hProcess, current_affinity))
                sLog.outString("Using processors (bitmask, hex): %x", current_affinity);
            else
                sLog.outError("Can't set used processors (hex): %x", current_affinity);
        }
    }

    if (high_priority)
    {
        if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
            sLog.outString("Service process priority class set to HIGH");
        else
            sLog.outError("Can't set service process priority class.");
    }
#endif
}

} // namespace helper
} // namespace cmangos

#endif // PROCESS_PRIORITY_H_