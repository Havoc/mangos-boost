#ifndef BOOST_EXTENSION_COMPUTER_HPP
#define BOOST_EXTENSION_COMPUTER_HPP
#include <boost/extension/extension.hpp>
#include <iostream>
#include <typeinfo>
class BOOST_EXTENSION_COMPUTER_DECL computer
{
public:
  computer(void){std::cout << "\nCreated a Computer";}
  virtual ~computer(void){std::cout << "\nDestroyed a Computer";}
  virtual std::string list_capabilities(void);
};

#endif