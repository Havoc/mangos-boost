#ifndef BOOST_EXTENSION_VEHICLE_HPP
#define BOOST_EXTENSION_VEHICLE_HPP
#include <boost/extension/extension.hpp>
#include <iostream>
#include <typeinfo>
class BOOST_EXTENSION_VEHICLE_DECL vehicle
{
public:
  vehicle(void){std::cout << "\nCreated a Vehicle";}
  virtual ~vehicle(void){std::cout << "\nDestroyed a Vehicle";}
  virtual std::string list_capabilities(void);
};

#endif