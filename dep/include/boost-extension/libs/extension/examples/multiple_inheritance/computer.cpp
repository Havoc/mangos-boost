/* The following lines are only necessary because when
are linking to this dll at compile time with another
dll on Windows. As such, standard __declspec stuff
is required.

This example is something of a special case - normally
these types of macros are not necessary for classes 
- see the FAQ.
  */
#include <boost/extension/extension.hpp>
#define BOOST_EXTENSION_COMPUTER_DECL BOOST_EXTENSION_EXPORT_DECL


#include "computer.hpp"
#include <boost/extension/factory_map.hpp>

std::string computer::list_capabilities()
{
  return "\nIt computes.";
}

extern "C" void BOOST_EXTENSION_EXPORT_DECL extension_export(boost::extensions::factory_map & z)
{
  z.add<computer, computer, std::string>("\nA computer exported as a computer");
}