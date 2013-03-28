/* The following lines are only necessary because when
are linking to this dll at compile time with another
dll on Windows. As such, standard __declspec stuff
is required.

This example is something of a special case - normally
these types of macros are not necessary for classes 
- see the FAQ.
  */
#include <boost/extension/extension.hpp>
#define BOOST_EXTENSION_FLYING_CAR_DECL BOOST_EXTENSION_EXPORT_DECL


#include "flying_car.hpp"
#include <boost/extension/factory_map.hpp>

std::string flying_car::list_capabilities()
{
  return car::list_capabilities() + plane::list_capabilities() + "\nIt takes off from your driveway";
}

extern "C" void BOOST_EXTENSION_EXPORT_DECL extension_export(boost::extensions::factory_map & z)
{
  z.add<flying_car, vehicle, std::string>("A flying car exported as a vehicle");
  z.add<flying_car, plane, std::string>("A flying car exported as a plane");
  z.add<flying_car, car, std::string>("A flying car exported as a car");
  z.add<flying_car, flying_car, std::string>("A flying car exported as a flying car");
}