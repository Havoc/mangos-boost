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

#ifndef MANGOS_DEFINE_H
#define MANGOS_DEFINE_H

#include <sys/types.h>

#include <ace/Default_Constants.h>

#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>

#include <boost-extension/boost/extension/impl/linked_library.hpp>

#include <boost/detail/endian.hpp>

#include "Platform/CompilerDefs.h"

#define MANGOS_LITTLEENDIAN 0
#define MANGOS_BIGENDIAN    1

#if !defined(MANGOS_ENDIAN)
#  if defined (BOOST_BIG_ENDIAN)
#    define MANGOS_ENDIAN MANGOS_BIGENDIAN
#  elif defined (BOOST_LITTLE_ENDIAN)
#    define MANGOS_ENDIAN MANGOS_LITTLEENDIAN
#  else
#    error "Unsuported endianess"
#  endif
#endif // MANGOS_ENDIAN

typedef boost::extensions::impl::library_handle MANGOS_LIBRARY_HANDLE;

#define MANGOS_SCRIPT_NAME "mangosscript"
#define MANGOS_SCRIPT_SUFFIX "." BOOST_EXTENSION_LIBRARY_EXTENSION
// boost::extensions::shared_library code missing prefix define
#ifndef BOOST_WINDOWS
#   define MANGOS_SCRIPT_PREFIX "lib"
#else
#   define MANGOS_SCRIPT_PREFIX ""
#endif
#define MANGOS_LOAD_LIBRARY(libname)    boost::extensions::impl::load_shared_library(libname)
#define MANGOS_CLOSE_LIBRARY(hlib)      boost::extensions::impl::close_shared_library(hlib)
#define MANGOS_GET_PROC_ADDR(hlib,name) boost::extensions::impl::get_function(hlib,name)

#define MANGOS_PATH_MAX PATH_MAX                            // ace/os_include/os_limits.h -> ace/Basic_Types.h

#if PLATFORM == PLATFORM_WINDOWS
#  define MANGOS_EXPORT __declspec(dllexport)
#  define MANGOS_IMPORT __cdecl
#else // PLATFORM != PLATFORM_WINDOWS
#  define MANGOS_EXPORT export
#  if defined(__APPLE_CC__) && defined(BIG_ENDIAN)
#    define MANGOS_IMPORT __attribute__ ((longcall))
#  elif defined(__x86_64__)
#    define MANGOS_IMPORT
#  else
#    define MANGOS_IMPORT __attribute__ ((cdecl))
#  endif //__APPLE_CC__ && BIG_ENDIAN
#endif // PLATFORM

#if PLATFORM == PLATFORM_WINDOWS
#  ifdef MANGOS_WIN32_DLL_IMPORT
#    define MANGOS_DLL_DECL __declspec(dllimport)
#  else //!MANGOS_WIN32_DLL_IMPORT
#    ifdef MANGOS_WIND_DLL_EXPORT
#      define MANGOS_DLL_DECL __declspec(dllexport)
#    else //!MANGOS_WIND_DLL_EXPORT
#      define MANGOS_DLL_DECL
#    endif // MANGOS_WIND_DLL_EXPORT
#  endif // MANGOS_WIN32_DLL_IMPORT
#else // PLATFORM != PLATFORM_WINDOWS
#  define MANGOS_DLL_DECL
#endif // PLATFORM

#if PLATFORM == PLATFORM_WINDOWS
#  define MANGOS_DLL_SPEC __declspec(dllexport)
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif // DECLSPEC_NORETURN
#else // PLATFORM != PLATFORM_WINDOWS
#  define MANGOS_DLL_SPEC
#  define DECLSPEC_NORETURN
#endif // PLATFORM

#if !defined(DEBUG)
#  define MANGOS_INLINE inline
#else // DEBUG
#  if !defined(MANGOS_DEBUG)
#    define MANGOS_DEBUG
#  endif // MANGOS_DEBUG
#  define MANGOS_INLINE
#endif //!DEBUG

#if COMPILER == COMPILER_GNU
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F,V) __attribute__ ((format (printf, F, V)))
#else // COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#endif // COMPILER == COMPILER_GNU

typedef boost::int64_t int64;
typedef boost::int32_t int32;
typedef boost::int16_t int16;
typedef boost::int8_t int8;
typedef boost::uint64_t uint64;
typedef boost::uint32_t uint32;
typedef boost::uint16_t uint16;
typedef boost::uint8_t uint8;

#if COMPILER != COMPILER_MICROSOFT
typedef uint16      WORD;
typedef uint32      DWORD;
#endif // COMPILER

#if COMPILER == COMPILER_GNU
#  if !defined(__GXX_EXPERIMENTAL_CXX0X__) || (__GNUC__ < 4) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 7)
#    define override
#  endif
#endif

#ifdef BOOST_NO_CXX11_STATIC_ASSERT
#  define static_assert(a, b) BOOST_STATIC_ASSERT_MSG((a), b)
#endif

typedef uint64 OBJECT_HANDLE;

#endif // MANGOS_DEFINE_H
