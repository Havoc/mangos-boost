/* The following lines are only necessary because when
are linking to this dll at compile time with another
dll on Windows. As such, standard __declspec stuff
is required.

This example is something of a special case - normally
these types of macros are not necessary for classes 
- see the FAQ.
  */
#include <boost/extension/extension.hpp>
#define BOOST_EXTENSION_PLANE_DECL BOOST_EXTENSION_EXPORT_DECL


#include "plane.hpp"
#include <boost/extension/factory_map.hpp>

std::string plane::list_capabilities()
{
  return "\nIt flies in the air.";
}

extern "C" void BOOST_EXTENSION_EXPORT_DECL extension_export(boost::extensions::factory_map & z)
{
  z.add<plane, vehicle, std::string>("A plane exported as a vehicle");
  z.add<plane, plane, std::string>("A plane exported as a plane");
}