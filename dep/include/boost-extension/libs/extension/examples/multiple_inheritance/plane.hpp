#ifndef BOOST_EXTENSION_PLANE_HPP
#define BOOST_EXTENSION_PLANE_HPP
//  See the FAQ for info about why the following is necessary
//  here, but usually isn't.
#define BOOST_EXTENSION_VEHICLE_DECL BOOST_EXTENSION_IMPORT_DECL
#include "vehicle.hpp"
#include <iostream>
#include <typeinfo>
class  BOOST_EXTENSION_PLANE_DECL plane : virtual public vehicle
{
public:
  plane(void){std::cout << "\nCreated a Plane";}
  ~plane(void){std::cout << "\nDestroyed a Plane";}
  virtual std::string list_capabilities(void);
};

#endif