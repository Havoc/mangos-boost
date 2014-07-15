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

set(LIBRARY_NAME cmangos_library_framework)

set(SRC_GRP_CONFIGURATION
    configuration/Config.cpp
    configuration/Config.h
)

set(SRC_GRP_CRYPTOGRAPHY
    cryptography/AuthCrypt.cpp
    cryptography/AuthCrypt.h
    cryptography/BigNumber.cpp
    cryptography/BigNumber.h
    cryptography/HMACSHA1.cpp
    cryptography/HMACSHA1.h
    cryptography/md5.h
    cryptography/SARC4.cpp
    cryptography/SARC4.h
    cryptography/Sha1.cpp
    cryptography/Sha1.h
)

set(SRC_GRP_DEBUGGING
    debugging/Errors.h
)

set(SRC_GRP_DATABASE
    database/Database.cpp
    database/Database.h
    database/DatabaseEnv.h
    database/DatabaseImpl.h
    database/DatabaseMysql.cpp
    database/DatabaseMysql.h
    database/DatabasePostgre.cpp
    database/DatabasePostgre.h
    database/Field.cpp
    database/Field.h
    database/PGSQLDelayThread.h
    database/QueryResult.h
    database/QueryResultMysql.cpp
    database/QueryResultMysql.h
    database/QueryResultPostgre.cpp
    database/QueryResultPostgre.h
    database/SqlDelayThread.cpp
    database/SqlDelayThread.h
    database/SqlOperations.cpp
    database/SqlOperations.h
    database/SqlPreparedStatement.cpp
    database/SqlPreparedStatement.h
    database/SQLStorage.cpp
    database/SQLStorage.h
    database/SQLStorageImpl.h
)

set(SRC_GRP_DATABASE_DBC
    database/DBCFileLoader.cpp
    database/DBCFileLoader.h
    database/DBCStore.h
)

set(SRC_GRP_DYNAMIC
    dynamic/FactoryHolder.h
    dynamic/ObjectRegistry.h
)

set(SRC_GRP_GAMESYSTEM
    game_system/Grid.h
    game_system/GridLoader.h
    game_system/GridReference.h
    game_system/GridRefManager.h
    game_system/NGrid.h
    game_system/TypeContainer.h
    game_system/TypeContainerFunctions.h
    game_system/TypeContainerVisitor.h
)

set(SRC_GRP_HELPER
    helper/ConsoleArguments.h
    helper/Hints.h
    helper/ProcessPriority.h
)

set(SRC_GRP_LOG
    log/Log.cpp
    log/Log.h
)

set(SRC_GRP_NETWORK
    network/ByteBuffer.cpp
    network/ByteBuffer.h
    network/NetworkBuffer.cpp
    network/NetworkBuffer.h
    network/NetworkManager.cpp
    network/NetworkManager.h
    network/NetworkThread.cpp
    network/NetworkThread.h
    network/ProtocolDefinitions.h
    network/Socket.cpp
    network/Socket.h
    network/WorldPacket.h
)

set(SRC_GRP_PLATFORM
    platform/CompilerDefs.h
    platform/Define.h
)

set(SRC_GRP_POLICIES
    policies/CreationPolicy.h
    policies/MemoryManagement.cpp
    policies/ObjectLifeTime.cpp
    policies/ObjectLifeTime.h
    policies/Singleton.h
    policies/ThreadingModel.h
)

set(SRC_GRP_SYSTEM
    system/ProgressBar.cpp
    system/ProgressBar.h
    system/SystemConfig.h
)

set(SRC_GRP_THREADING
    threading/LockedQueue.h
    threading/Threading.cpp
    threading/Threading.h
)

set(SRC_GRP_UTILITIES
    utilities/ByteConverter.h
    utilities/Callback.h
    utilities/EventProcessor.cpp
    utilities/EventProcessor.h
    utilities/LinkedList.h
    utilities/Timer.cpp
    utilities/Timer.h
    utilities/TypeList.h
    utilities/UnorderedMapSet.h
    utilities/Util.cpp
    utilities/Util.h
)

set(SRC_GRP_LINKED_REFERENCE
    utilities/LinkedReference/Reference.h
    utilities/LinkedReference/RefManager.h
)

# OS specific files
if(WIN32)
  set(SRC_GRP_DEBUGGING
      ${SRC_GRP_DEBUGGING}
      debugging/WheatyExceptionReport.cpp
      debugging/WheatyExceptionReport.h
     )
  set(SRC_GRP_SYSTEM
      ${SRC_GRP_SYSTEM}
      system/ServiceWin32.cpp
      system/ServiceWin32.h
     )
else()
  set(SRC_GRP_SYSTEM
      ${SRC_GRP_SYSTEM}
      system/PosixDaemon.cpp
      system/PosixDaemon.h
     )
endif()

set(LIBRARY_SRCS
    ${SRC_GRP_CONFIGURATION}
    ${SRC_GRP_CRYPTOGRAPHY}
    ${SRC_GRP_DATABASE}
    ${SRC_GRP_DEBUGGING}
    ${SRC_GRP_DYNAMIC}
    ${SRC_GRP_GAMESYSTEM}
    ${SRC_GRP_HELPER}
    ${SRC_GRP_LOG}
    ${SRC_GRP_NETWORK}
    ${SRC_GRP_PLATFORM}
    ${SRC_GRP_POLICIES}
    ${SRC_GRP_SYSTEM}
    ${SRC_GRP_THREADING}
    ${SRC_GRP_UTILITIES}
    ${SRC_GRP_LINKED_REFERENCE}
    Common.cpp
    Common.h
    revision_nr.h
    revision_sql.h
)

source_group("Configuration" FILES ${SRC_GRP_CONFIGURATION})
source_group("Cryptography" FILES ${SRC_GRP_CRYPTOGRAPHY})
source_group("Database" FILES ${SRC_GRP_DATABASE})
source_group("Database\\DataStores" FILES ${SRC_GRP_DATABASE_DBC})
source_group("Debugging" FILES ${SRC_GRP_DEBUGGING})
source_group("Dynamic" FILES ${SRC_GRP_DYNAMIC})
source_group("GameSystem" FILES ${SRC_GRP_GAMESYSTEM})
source_group("Helper" FILES ${SRC_GRP_HELPER})
source_group("Log" FILES ${SRC_GRP_LOG})
source_group("Network" FILES ${SRC_GRP_NETWORK})
source_group("Platform" FILES ${SRC_GRP_PLATFORM})
source_group("Policies" FILES ${SRC_GRP_POLICIES})
source_group("System" FILES ${SRC_GRP_SYSTEM})
source_group("Threading" FILES ${SRC_GRP_THREADING})
source_group("Utilities" FILES ${SRC_GRP_UTILITIES})
source_group("Utilities\\LinkedReference" FILES ${SRC_GRP_LINKED_REFERENCE})

include_directories(
  ${ACE_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/dep/include
  ${CMAKE_SOURCE_DIR}/dep/backtrace
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/configuration
  ${CMAKE_CURRENT_SOURCE_DIR}/cryptography
  ${CMAKE_CURRENT_SOURCE_DIR}/database
  ${CMAKE_CURRENT_SOURCE_DIR}/debugging
  ${CMAKE_CURRENT_SOURCE_DIR}/dynamic
  ${CMAKE_CURRENT_SOURCE_DIR}/game_system
  ${CMAKE_CURRENT_SOURCE_DIR}/log
  ${CMAKE_CURRENT_SOURCE_DIR}/network
  ${CMAKE_CURRENT_SOURCE_DIR}/platform
  ${CMAKE_CURRENT_SOURCE_DIR}/policies
  ${CMAKE_CURRENT_SOURCE_DIR}/system
  ${CMAKE_CURRENT_SOURCE_DIR}/threading
  ${CMAKE_CURRENT_SOURCE_DIR}/utilities
  ${CMAKE_BINARY_DIR}
)

if(NOT USE_STD_MALLOC)
  include_directories(${TBB_INCLUDE_DIR})
endif()

if(POSTGRESQL)
  include_directories(${PostgreSQL_INCLUDE_DIRS})
else()
  include_directories(${MYSQL_INCLUDE_DIR})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/system/SystemConfig.h.in ${CMAKE_BINARY_DIR}/SystemConfig.h)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SRCS})
set_target_properties(${LIBRARY_NAME} PROPERTIES FOLDER Libraries)

add_dependencies(${LIBRARY_NAME} revision.h)
if(WIN32)
  add_dependencies(${LIBRARY_NAME} cmangos_dependency_ace)
endif()

if(NOT USE_STD_MALLOC)
  target_link_libraries(${LIBRARY_NAME}
    optimized ${TBB_LIBRARIES}
    debug ${TBB_LIBRARIES_DEBUG}
  )
endif()

if(POSTGRESQL)
  target_link_libraries(${LIBRARY_NAME} ${PostgreSQL_LIBRARIES})
endif()
