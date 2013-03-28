#ifndef BOOST_EXTENSION_CAR_HPP
#define BOOST_EXTENSION_CAR_HPP
//  See the FAQ for info about why the following is necessary
//  here, but usually isn't.
#define BOOST_EXTENSION_VEHICLE_DECL BOOST_EXTENSION_IMPORT_DECL
#include "vehicle.hpp"
#include <iostream>
#include <typeinfo>
class BOOST_EXTENSION_CAR_DECL car : virtual public vehicle
{
public:
  car(void){std::cout << "\nCreated a Car";}
  virtual ~car(void){std::cout << "\nDestroyed a Car";}
  virtual std::string list_capabilities(void);
};

#endif