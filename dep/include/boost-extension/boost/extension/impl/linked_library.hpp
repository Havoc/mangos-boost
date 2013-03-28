#define BOOST_EXTENSIONS_IMPL_NAMESPACE_BEGIN namespace boost{ namespace extensions{ namespace impl {
#define BOOST_EXTENSIONS_IMPL_NAMESPACE_END }}} // ns: boost::extensions::impl

#ifdef BOOST_WINDOWS

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef WINDOWS_LEAN_AND_MEAN
#define WINDOWS_LEAN_AND_MEAN
#endif

#include <Windows.h>

#define BOOST_EXTENSION_LIBRARY_EXTENSION "dll"

BOOST_EXTENSIONS_IMPL_NAMESPACE_BEGIN

  typedef HMODULE library_handle;
  typedef FARPROC generic_function_ptr;

  inline bool is_library(const char * file_name) {
      size_t len = strlen(file_name);
      return (len > 3 &&  file_name[len-4] == '.'
          && file_name[len-3] == 'd' && file_name[len-2] == 'l' && file_name[len-1] == 'l');
  }

  inline library_handle load_shared_library(const char * libraryName) {
      return LoadLibrary(libraryName);
  }

  inline generic_function_ptr get_function(library_handle handle, const char * function_name) {
      return GetProcAddress(handle, function_name);
  }

  inline bool close_shared_library(library_handle handle){
      return FreeLibrary(handle) != 0;
  }

BOOST_EXTENSIONS_IMPL_NAMESPACE_END

#pragma comment(lib, "kernel32.lib")

#else

#ifdef __APPLE__

#define BOOST_EXTENSION_LIBRARY_EXTENSION "dylib"

BOOST_EXTENSIONS_IMPL_NAMESPACE_BEGIN

  inline bool is_library(const char * file_name)
  {
    size_t len = strlen(file_name);
    return ((len > 5 && file_name[len-7] == '.' && file_name[len-6] == 'b' && file_name[len-5] == 'u' &&
                     file_name[len-4] == 'n' && file_name[len-3] == 'd' && file_name[len-2] == 'l' &&
                     file_name[len-1] == 'e') || (len > 4 && file_name[len-6] == '.' && 
                     file_name[len-5] == 'd' && file_name[len-4] == 'y' && file_name[len-3] == 'l' && 
                     file_name[len-2] == 'i' && file_name[len-1] == 'b'));
  }
BOOST_EXTENSIONS_IMPL_NAMESPACE_END

#else

#define BOOST_EXTENSION_LIBRARY_EXTENSION "so"

BOOST_EXTENSIONS_IMPL_NAMESPACE_BEGIN

  inline bool is_library(const char * file_name) {
    size_t len = strlen(file_name);
    return (len > 2 && file_name[len-3] == '.'
        && file_name[len-2] == 's' && file_name[len-1] == 'o');
  }

BOOST_EXTENSIONS_IMPL_NAMESPACE_END

#endif

#include <dlfcn.h>

BOOST_EXTENSIONS_IMPL_NAMESPACE_BEGIN

  typedef void * library_handle;
  typedef void * generic_function_ptr;

  inline library_handle load_shared_library(const char * library_name) {
      return dlopen(library_name, RTLD_LAZY);
  }

  inline generic_function_ptr get_function(library_handle handle, const char * function_name) {
      return dlsym(handle, function_name);
  }

  inline bool close_shared_library(library_handle handle) {
      return dlclose(handle) == 0;
  }

BOOST_EXTENSIONS_IMPL_NAMESPACE_END
#endif

#undef BOOST_EXTENSIONS_IMPL_NAMESPACE_BEGIN
#undef BOOST_EXTENSIONS_IMPL_NAMESPACE_END

