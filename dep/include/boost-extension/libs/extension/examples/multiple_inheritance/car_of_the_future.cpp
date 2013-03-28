/* The following lines are only necessary because when
are linking to this dll at compile time with another
dll on Windows. As such, standard __declspec stuff
is required.

This example is something of a special case - normally
these types of macros are not necessary for classes 
- see the FAQ.
  */
#include <boost/extension/extension.hpp>
#define BOOST_EXTENSION_CAR_OF_THE_FUTURE_DECL BOOST_EXTENSION_EXPORT_DECL

#include "car_of_the_future.hpp"
#include <boost/extension/factory_map.hpp>

std::string car_of_the_future::list_capabilities()
{
  return boat::list_capabilities() + flying_car::list_capabilities() +
         computer::list_capabilities() + "\nCosts an arm and a leg";
}

extern "C" void BOOST_EXTENSION_EXPORT_DECL extension_export(boost::extensions::factory_map & z)
{
  z.add<car_of_the_future, vehicle, std::string>
    ("A car of the future exported as a vehicle");
  z.add<car_of_the_future, car, std::string>
    ("A car of the future exported as a car");
  z.add<car_of_the_future, plane, std::string>
    ("A car of the future exported as a plane");
  z.add<car_of_the_future, flying_car, std::string>
    ("A car of the future exported as a flying car");
  z.add<car_of_the_future, boat, std::string>
    ("A car of the future exported as a boat");
  z.add<car_of_the_future, computer, std::string>
    ("A car of the future exported as a computer");
  z.add<car_of_the_future, car_of_the_future, std::string>
    ("A car of the future exported as a car of the future");
}