#
# This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

# define all options here
option(DEBUG                "Debug mode"                            OFF)
option(USE_STD_MALLOC       "Use standard malloc instead of TBB"    OFF)
option(POSTGRESQL           "Use PostgreSQL"                        OFF)

# TODO: why only enable it on windows by default?
if(PCHSupport_FOUND AND WIN32) 
  option(PCH                "Use precompiled headers"               ON)
else()
  option(PCH                "Use precompiled headers"               OFF)
endif()

# TODO: options that should be checked/created:
#option(CLI                  "With CLI"                             ON)
#option(RA                   "With Remote Access"                   OFF)
#option(SQL                  "Copy SQL files"                       OFF)
#option(TOOLS                "Build tools"                          OFF)

# Output description of this script
message("This script builds the CMaNGOS server.

Options that can be used in order to configure the process:

    CMAKE_INSTALL_PREFIX    Path where the server should be installed to
    PCH                     Use precompiled headers
    DEBUG                   Debug mode
    INCLUDE_BINDINGS_DIR    Include a script library in src/bindings/ with the
                            defined name. the name must corespond to the name of
                            the folder and the folder must contain a valid
                            CMakeLists.txt
    USE_STD_MALLOC          Use standard malloc instead of TBB

To set an option simply type -D<OPTION>=<VALUE> after 'cmake <srcs>'.
Also, you can specify the generator with -G. see 'cmake --help' for more details
For example: cmake .. -DDEBUG=1 -DCMAKE_INSTALL_PREFIX=/opt/mangos
")