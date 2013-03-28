/* (C) Copyright Jeremy Pack 2007
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_EXTENSION_FACTORY_MAP_HPP
#define BOOST_EXTENSION_FACTORY_MAP_HPP
#include <list>
#include <memory>
#include <map>
#include <boost/extension/factory.hpp>
#include <boost/extension/extension.hpp>
#include  <boost/extension/impl/typeinfo.hpp>

namespace boost{namespace extensions{


template <class TypeInfo>
class basic_factory_map
{
private:
  class generic_factory_container
  {
  public:
    virtual ~generic_factory_container(){}
  };
  template <class Interface, class Info, class Param1 = void, class Param2 = void, class Param3 = void, class Param4 = void, class Param5 = void, class Param6 = void>
  class factory_container : public std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> >, public generic_factory_container
  {
  public:
    factory_container(){}
    factory_container(basic_factory_map & z)
      :std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> >(z.get<Interface, Param1, Param2, Param3, Param4, Param5, Param6>()){}
    virtual ~factory_container(){}
  };
  typedef std::map<TypeInfo, generic_factory_container *> FactoryMap;
  FactoryMap factories_; 
public:
  ~basic_factory_map(){
    for(typename FactoryMap::iterator it = factories_.begin(); it != factories_.end(); ++it)
      delete it->second;
    //TODO - test for memory leaks.
  }
  template <class Interface, class Info>
operator std::list<factory<Interface, Info> > & ()
  {return this->get<Interface, Info>();}
  template <class Interface, class Info>
std::list<factory<Interface, Info> > & get()
  {
      TypeInfo current_type = 
         type_info_handler<TypeInfo, factory<Interface, Info> >::get_class_type();
      typename FactoryMap::iterator it = 
        factories_.find(current_type);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info> * ret = 
          new factory_container<Interface, Info>();
        factories_[current_type] = ret;
        return *ret;
      }
      else
      {
        // Change to dynamic if this fails
        return static_cast<factory_container<Interface, Info> &>(*(it->second));
      }
  }
template <class Actual, class Interface, class Info>
void add(Info info)
  {
    typedef std::list<factory<Interface, Info> > ListType;
    ListType & s = this->get<Interface, Info>();
    factory<Interface, Info> f(info);
    //f.set_type<Actual>();
    f.set_type_special((Actual*)0);
    s.push_back(f);
    //it->set_type<Actual>(); 
  }
  template <class Interface, class Info, class Param1>
operator std::list<factory<Interface, Info, Param1> > & ()
  {return this->get<Interface, Info, Param1>();}
  template <class Interface, class Info, class Param1>
std::list<factory<Interface, Info, Param1> > & get()
  {
      TypeInfo current_type = 
         type_info_handler<TypeInfo, factory<Interface, Info, Param1> >::get_class_type();
      typename FactoryMap::iterator it = 
        factories_.find(current_type);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1> * ret = 
          new factory_container<Interface, Info, Param1>();
        factories_[current_type] = ret;
        return *ret;
      }
      else
      {
        // Change to dynamic if this fails
        return static_cast<factory_container<Interface, Info, Param1> &>(*(it->second));
      }
  }
template <class Actual, class Interface, class Info, class Param1>
void add(Info info)
  {
    typedef std::list<factory<Interface, Info, Param1> > ListType;
    ListType & s = this->get<Interface, Info, Param1>();
    factory<Interface, Info, Param1> f(info);
    //f.set_type<Actual>();
    f.set_type_special((Actual*)0);
    s.push_back(f);
    //it->set_type<Actual>(); 
  }
  template <class Interface, class Info, class Param1, class Param2>
operator std::list<factory<Interface, Info, Param1, Param2> > & ()
  {return this->get<Interface, Info, Param1, Param2>();}
  template <class Interface, class Info, class Param1, class Param2>
std::list<factory<Interface, Info, Param1, Param2> > & get()
  {
      TypeInfo current_type = 
         type_info_handler<TypeInfo, factory<Interface, Info, Param1, Param2> >::get_class_type();
      typename FactoryMap::iterator it = 
        factories_.find(current_type);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2> * ret = 
          new factory_container<Interface, Info, Param1, Param2>();
        factories_[current_type] = ret;
        return *ret;
      }
      else
      {
        // Change to dynamic if this fails
        return static_cast<factory_container<Interface, Info, Param1, Param2> &>(*(it->second));
      }
  }
template <class Actual, class Interface, class Info, class Param1, class Param2>
void add(Info info)
  {
    typedef std::list<factory<Interface, Info, Param1, Param2> > ListType;
    ListType & s = this->get<Interface, Info, Param1, Param2>();
    factory<Interface, Info, Param1, Param2> f(info);
    //f.set_type<Actual>();
    f.set_type_special((Actual*)0);
    s.push_back(f);
    //it->set_type<Actual>(); 
  }
  template <class Interface, class Info, class Param1, class Param2, class Param3>
operator std::list<factory<Interface, Info, Param1, Param2, Param3> > & ()
  {return this->get<Interface, Info, Param1, Param2, Param3>();}
  template <class Interface, class Info, class Param1, class Param2, class Param3>
std::list<factory<Interface, Info, Param1, Param2, Param3> > & get()
  {
      TypeInfo current_type = 
         type_info_handler<TypeInfo, factory<Interface, Info, Param1, Param2, Param3> >::get_class_type();
      typename FactoryMap::iterator it = 
        factories_.find(current_type);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3> * ret = 
          new factory_container<Interface, Info, Param1, Param2, Param3>();
        factories_[current_type] = ret;
        return *ret;
      }
      else
      {
        // Change to dynamic if this fails
        return static_cast<factory_container<Interface, Info, Param1, Param2, Param3> &>(*(it->second));
      }
  }
template <class Actual, class Interface, class Info, class Param1, class Param2, class Param3>
void add(Info info)
  {
    typedef std::list<factory<Interface, Info, Param1, Param2, Param3> > ListType;
    ListType & s = this->get<Interface, Info, Param1, Param2, Param3>();
    factory<Interface, Info, Param1, Param2, Param3> f(info);
    //f.set_type<Actual>();
    f.set_type_special((Actual*)0);
    s.push_back(f);
    //it->set_type<Actual>(); 
  }
  template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4>
operator std::list<factory<Interface, Info, Param1, Param2, Param3, Param4> > & ()
  {return this->get<Interface, Info, Param1, Param2, Param3, Param4>();}
  template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4>
std::list<factory<Interface, Info, Param1, Param2, Param3, Param4> > & get()
  {
      TypeInfo current_type = 
         type_info_handler<TypeInfo, factory<Interface, Info, Param1, Param2, Param3, Param4> >::get_class_type();
      typename FactoryMap::iterator it = 
        factories_.find(current_type);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3, Param4> * ret = 
          new factory_container<Interface, Info, Param1, Param2, Param3, Param4>();
        factories_[current_type] = ret;
        return *ret;
      }
      else
      {
        // Change to dynamic if this fails
        return static_cast<factory_container<Interface, Info, Param1, Param2, Param3, Param4> &>(*(it->second));
      }
  }
template <class Actual, class Interface, class Info, class Param1, class Param2, class Param3, class Param4>
void add(Info info)
  {
    typedef std::list<factory<Interface, Info, Param1, Param2, Param3, Param4> > ListType;
    ListType & s = this->get<Interface, Info, Param1, Param2, Param3, Param4>();
    factory<Interface, Info, Param1, Param2, Param3, Param4> f(info);
    //f.set_type<Actual>();
    f.set_type_special((Actual*)0);
    s.push_back(f);
    //it->set_type<Actual>(); 
  }
  template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4, class Param5>
operator std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5> > & ()
  {return this->get<Interface, Info, Param1, Param2, Param3, Param4, Param5>();}
  template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4, class Param5>
std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5> > & get()
  {
      TypeInfo current_type = 
         type_info_handler<TypeInfo, factory<Interface, Info, Param1, Param2, Param3, Param4, Param5> >::get_class_type();
      typename FactoryMap::iterator it = 
        factories_.find(current_type);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5> * ret = 
          new factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5>();
        factories_[current_type] = ret;
        return *ret;
      }
      else
      {
        // Change to dynamic if this fails
        return static_cast<factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5> &>(*(it->second));
      }
  }
template <class Actual, class Interface, class Info, class Param1, class Param2, class Param3, class Param4, class Param5>
void add(Info info)
  {
    typedef std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5> > ListType;
    ListType & s = this->get<Interface, Info, Param1, Param2, Param3, Param4, Param5>();
    factory<Interface, Info, Param1, Param2, Param3, Param4, Param5> f(info);
    //f.set_type<Actual>();
    f.set_type_special((Actual*)0);
    s.push_back(f);
    //it->set_type<Actual>(); 
  }
  template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6>
operator std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> > & ()
  {return this->get<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6>();}
  template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6>
std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> > & get()
  {
      TypeInfo current_type = 
         type_info_handler<TypeInfo, factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> >::get_class_type();
      typename FactoryMap::iterator it = 
        factories_.find(current_type);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> * ret = 
          new factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6>();
        factories_[current_type] = ret;
        return *ret;
      }
      else
      {
        // Change to dynamic if this fails
        return static_cast<factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> &>(*(it->second));
      }
  }
template <class Actual, class Interface, class Info, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6>
void add(Info info)
  {
    typedef std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> > ListType;
    ListType & s = this->get<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6>();
    factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> f(info);
    //f.set_type<Actual>();
    f.set_type_special((Actual*)0);
    s.push_back(f);
    //it->set_type<Actual>(); 
  }
};

typedef basic_factory_map<default_type_info> factory_map;
}}

#endif
