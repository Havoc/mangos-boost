
/* The following lines are only necessary because when
are linking to this dll at compile time with another
dll on Windows. As such, standard __declspec stuff
is required.

This example is something of a special case - normally
these types of macros are not necessary for classes 
- see the FAQ.
  */
#include <boost/extension/extension.hpp>
#define BOOST_EXTENSION_BOAT_DECL BOOST_EXTENSION_EXPORT_DECL
#include "boat.hpp"
#include <boost/extension/factory_map.hpp>


std::string boat::list_capabilities()
{
  return "\nIt floats on water.";
}

extern "C" void BOOST_EXTENSION_EXPORT_DECL extension_export(boost::extensions::factory_map & z)
{
  z.add<boat, vehicle, std::string>("A boat exported as a vehicle");
  z.add<boat, boat, std::string>("A boat exported as a boat");
}