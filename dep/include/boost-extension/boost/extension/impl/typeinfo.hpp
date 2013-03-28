#ifndef BOOST_EXTENSION_TYPEINFO_HPP
#define BOOST_EXTENSION_TYPEINFO_HPP
namespace boost{namespace extensions{
template <class TypeInfo, class ClassType>
struct type_info_handler
{
  static TypeInfo get_class_type();
};
}}
#ifndef BOOST_EXTENSION_USER_TYPE_INFO_CUSTOM
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
 //  For DLLs
#  define BOOST_EXTENSION_EXTERNAL extern "C" __declspec(dllexport) 
#include <string>
#include <typeinfo>
namespace boost{namespace extensions{
  typedef std::string default_type_info;
  template <class ClassType>
    struct type_info_handler<default_type_info, ClassType>
  {
    static default_type_info get_class_type(){return typeid(ClassType).name();}
  };
}}

#else
#ifdef __APPLE__
#include <typeinfo>
#include <string>
/*namespace boost{namespace extensions{
  typedef basic_factory_map<std::type_info> factory_map;
  template <>
    std::type_info basic_factory_map<std::string>::get_class_type()
  {
      return typeid(ClassType);
  }
}}*/
namespace boost{namespace extensions{
  typedef std::string default_type_info;
  template <class ClassType>
    struct type_info_handler<default_type_info, ClassType>
  {
    static default_type_info get_class_type(){return typeid(ClassType).name();}
  };
}}

#else
#include <string>
#include <typeinfo>
namespace boost{namespace extensions{
  typedef std::string default_type_info;
  template <class ClassType>
    struct type_info_handler<default_type_info, ClassType>
  {
    static default_type_info get_class_type(){return typeid(ClassType).name();}
  };
}}

#endif //Apple
#endif //Windows
#endif // BOOST_EXTENSION_USER_TYPE_INFO_CUSTOM
#ifndef BOOST_EXTENSION_EXTERNAL
#  define BOOST_EXTENSION_EXTERNAL extern "C"
#endif

#endif