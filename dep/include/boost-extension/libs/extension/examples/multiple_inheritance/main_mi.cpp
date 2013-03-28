/* (C) Copyright Jeremy Pack 2007
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*/

#include <boost/extension/factory_map.hpp>
#include <boost/extension/shared_library.hpp>
//  See the FAQ for info about why the following is necessary
//  here, but usually isn't.
#define BOOST_EXTENSION_VEHICLE_DECL BOOST_EXTENSION_IMPORT_DECL
#define BOOST_EXTENSION_COMPUTER_DECL BOOST_EXTENSION_IMPORT_DECL
#include "vehicle.hpp"
#include "computer.hpp"
#include <iostream>
#include <boost/extension/convenience.hpp>
// #include <boost/extension/filesystem.hpp>
int main()
{
  using namespace boost::extensions;
  //  Create the factory_map object - it will hold all of the available
  //  constructors. Multiple zones can be constructed.
  factory_map twilight;
  //  Load the constructors and information into the factory_map.
  load_single_library(twilight, "libVehicle.extension", "extension_export");
  load_single_library(twilight, "libCar.extension", "extension_export");
  load_single_library(twilight, "libComputer.extension", "extension_export");
  load_single_library(twilight, "libBoat.extension", "extension_export");
  load_single_library(twilight, "libFlyingCar.extension", "extension_export");
  load_single_library(twilight, "libCarOfTheFuture.extension", "extension_export");
  load_single_library(twilight, "libPlane.extension", "extension_export");
  // load_all_libraries(twilight, "./", "extension_export_word");
  //  Get a reference to the list of constructors.
  //  Note that the factories can be copied just fine - meaning that the factory list
  //  can be copied from the factory_map object into a different data structure, and the factory_map
  //  can be destroyed.
  std::cout << "\n>>>>>>>>>>>>\nComputers:\n>>>>>>>>>>>>>>>>>>>";
  std::list<factory<computer, std::string> > & factory_list = twilight.get<computer, std::string>();  
  if (factory_list.size() < 1)
    std::cout << "Error - no computers were found.";
  for (std::list<factory<computer, std::string> >::iterator comp = factory_list.begin();
       comp != factory_list.end(); ++comp)
  {
    //  Using auto_ptr to avoid needing delete. Using smart_ptrs is recommended.
    //  Note that this has a zero argument constructor - currently constructors
    //  with up to six arguments can be used.
    std::auto_ptr<computer> computer_ptr(comp->create());
    std::cout << "\n--------\nLoaded the class described as: ";
    std::cout << comp->get_info();
    std::cout << "\n\nIt claims the following capabilities: ";
    std::cout << computer_ptr->list_capabilities() << "\n";
  }
  std::cout << "\n\n";
  
  
  
  std::cout << "\n>>>>>>>>>>>>\nVehicles:\n>>>>>>>>>>>>>>>>>>>";
  std::list<factory<vehicle, std::string> > & factory_list2 = twilight.get<vehicle, std::string>();  
  if (factory_list2.size() < 1)
    std::cout << "Error - no vehicles were found.";
  for (std::list<factory<vehicle, std::string> >::iterator comp = factory_list2.begin();
       comp != factory_list2.end(); ++comp)
  {
    //  Using auto_ptr to avoid needing delete. Using smart_ptrs is recommended.
    //  Note that this has a zero argument constructor - currently constructors
    //  with up to six arguments can be used.
    std::auto_ptr<vehicle> computer_ptr(comp->create());
    std::cout << "\n--------\nLoaded the class described as: ";
    std::cout << comp->get_info();
    std::cout << "\n\nIt claims the following capabilities: ";
    std::cout << computer_ptr->list_capabilities() << "\n";
  }
  std::cout << "\n\n";  
  return 0;
}