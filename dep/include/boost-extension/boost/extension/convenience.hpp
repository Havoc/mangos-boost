/* (C) Copyright Jeremy Pack 2007
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_EXTENSION_CONVENIENCE_HPP
#define BOOST_EXTENSION_CONVENIENCE_HPP

#include <boost/extension/factory_map.hpp>

namespace boost{namespace extensions{
  
inline void load_single_library(factory_map & current_zone, const char * library_path, const char * external_function_name)
{
  shared_library lib(library_path);
  if (!lib.open())
  {
    return;
  }
  functor<void, factory_map &> load_func = lib.get_functor<void, factory_map &>(external_function_name);
  if (!load_func.is_valid())
  {
    return;
  }
  load_func(current_zone);
}

}}



#endif
