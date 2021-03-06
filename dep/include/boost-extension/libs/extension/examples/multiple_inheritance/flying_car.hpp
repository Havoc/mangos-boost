#ifndef BOOST_EXTENSION_FLYING_CAR_HPP
#define BOOST_EXTENSION_FLYING_CAR_HPP
//  See the FAQ for info about why the following is necessary
//  here, but usually isn't.
#define BOOST_EXTENSION_PLANE_DECL BOOST_EXTENSION_IMPORT_DECL
#define BOOST_EXTENSION_CAR_DECL BOOST_EXTENSION_IMPORT_DECL
#include "car.hpp"
#include "plane.hpp"
#include <iostream>
#include <typeinfo>
class  BOOST_EXTENSION_FLYING_CAR_DECL flying_car : public car, public plane
{
public:
  flying_car(void):vehicle(){std::cout << "\nCreated a Flying Car";}
  ~flying_car(void){std::cout << "\nDestroyed a Flying Car";}
  virtual std::string list_capabilities(void);
};

#endif