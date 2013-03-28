/* (C) Copyright Jeremy Pack 2007
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "word.hpp"
#include <boost/extension/factory_map.hpp>

class world : public word
{
public:
  virtual const char * get_val(){return "world!";}
};
class hello : public word
{
public:
  virtual const char * get_val(){return "hello";}
};
extern "C" void BOOST_EXTENSION_EXPORT_DECL extension_export_word(boost::extensions::factory_map & fm)
{
  fm.add<hello, word, int>(1);
  fm.add<world, word, int>(2);
}