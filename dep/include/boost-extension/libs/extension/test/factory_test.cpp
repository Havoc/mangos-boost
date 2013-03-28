/* (C) Copyright Jeremy Pack 2007
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/extension/factory_map.hpp>
#include <boost/extension/factory.hpp>
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "fruit.hpp"
#include <vector>
#include <list>
using namespace boost::extensions;
void setup_zone(factory_map & z)
{
  z.add<apple, fruit, std::string, int, int>("round fruit");
  z.add<banana, fruit, std::string, int, int>("long fruit");
  z.add<nectarine, fruit, std::string, int, int>("small fruit");
  
}
BOOST_AUTO_TEST_CASE(construct_from_zone)
{
  factory_map z;
  setup_zone(z);
  std::vector<factory<fruit, std::string, int, int> > f1(z.get<fruit, std::string, int, int>().begin(), z.get<fruit, std::string, int, int>().end());
  std::list<factory<fruit, std::string, int, int> > f2(z.get<fruit, std::string, int, int>().begin(), z.get<fruit, std::string, int, int>().end());
  std::list<factory<fruit, std::string, int, int> > f3(z);
  std::list<factory<fruit, std::string, int, int> > f4 = z;
  BOOST_CHECK_EQUAL(f1.size(), f2.size());
  BOOST_CHECK_EQUAL(f1.size(), f3.size());
  BOOST_CHECK_EQUAL(f2.size(), f4.size());
  BOOST_CHECK_EQUAL(f1.size(), size_t(3));
}
BOOST_AUTO_TEST_CASE(factory_construction)
{
  factory_map z;
  setup_zone(z);
  std::vector<factory<fruit, std::string, int, int> > f1(z.get<fruit, std::string, int, int>().begin(), z.get<fruit, std::string, int, int>().end());
  std::vector<factory<fruit, std::string, int, int> >::iterator it = f1.begin();
  std::auto_ptr<fruit> first(it->create(0, 1));
  std::auto_ptr<fruit> second((++it)->create(0, 1));
  std::auto_ptr<fruit> third((++it)->create(0, 1));
  BOOST_CHECK_EQUAL((first->get_cost()), 21);
  BOOST_CHECK_EQUAL((second->get_cost()), 7);
  BOOST_CHECK_EQUAL((third->get_cost()), 18);
  BOOST_CHECK_EQUAL(typeid(*first.get()).name(), typeid(apple).name());
  BOOST_CHECK_EQUAL(typeid(*second.get()).name(), typeid(banana).name());
  BOOST_CHECK_EQUAL(typeid(*third.get()).name(), typeid(nectarine).name());
  BOOST_CHECK(typeid(*third.get()).name() != typeid(banana).name());
  BOOST_CHECK(typeid(*third.get()).name() != typeid(banana).name());
  //factory<fruit> f;
  
}
BOOST_AUTO_TEST_CASE(extension_construction)
{
  //extension e;
  
}
