/* (C) Copyright Jeremy Pack 2007
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*/

#include <boost/extension/registry.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace boost::extensions;

BOOST_AUTO_TEST_CASE(registry_construction)
{
  registry reg;
  BOOST_CHECK_EQUAL(reg.num_libraries(), size_t(0));
  reg.open("../bin/libbasic_reg.extension");
  BOOST_CHECK_EQUAL(reg.num_libraries(), size_t(1));
  reg.close("../bin/libbasic_reg.extension");
  BOOST_CHECK_EQUAL(reg.num_libraries(), size_t(0));
}