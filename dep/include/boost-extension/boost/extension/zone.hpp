/* (C) Copyright Jeremy Pack 2007
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_EXTENSION_ZONE_HPP
#define BOOST_EXTENSION_ZONE_HPP
#include <list>
#include <memory>
#include <map>
#include <boost/extension/factory.hpp>


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
    factory_container(factory_map & z)
      :std::list<factory<Interface, Info, Param1, Param2> >(z.get<Interface, Param1, Param2, Param3, Param4, Param5, Param6>()){}
    virtual ~factory_container(){}
  };
  typedef std::map<TypeInfo, generic_factory_container *> FactoryMap;
  FactoryMap factories_;
  template <class ClassType>
    TypeInfo get_class_type();
public:
  ~factory_map(){
    for(FactoryMap::iterator it = factories_.begin(); it != factories_.end(); ++it)
      delete it->second;
    //TODO - test for memory leaks.
  }
  /*template <class Actual, class Interface, class Param1 = void, 
    class Param2 = void, class param3 = void, class Param4 = void, 
    class Param5 = void, class Param6 = void>
  extension & add(){}
  template <class Actual, class Interface, class Param1, 
    class Param2, class param3, class Param4, class Param5>
  extension & add(){}
  template <class Actual, class Interface, class Param1, 
    class Param2, class param3, class Param4>
  extension & add(){}
  template <class Actual, class Interface, class Param1, 
    class Param2, class param3>
  extension & add(){}
  template <class Actual, class Interface, class Param1, class Param2>
  extension & add(){}
  template <class Actual, class Interface, class Param1>
  extension & add(){}
  template <class Actual, class Interface>
  extension & add(){}*/
  
  
  /*template <class Interface, class Param1 = void, 
  class Param2 = void, class param3 = void, class Param4 = void, 
  class Param5 = void, class Param6 = void>
    const std::set<factory<Interface, Param1, Param2, Param3, Param4, Param5, Param6> & get(){}*/
  template <class Interface, class Info, class Param1, class Param2>
    operator std::list<factory<Interface, Info, Param1, Param2> > & ()
  {return this->get<Interface, Info, Param1, Param2>();}
  
  template <class Interface, class Info>
    std::list<factory<Interface, Info> > & get()
  {
      type_info current_type = 
        type_info::get_type_info<factory<Interface, Info> >();
      FactoryMap::iterator it = 
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
    std::list<factory<Interface, Info, Param1> > & get()
  {
      const std::type_info * current_type_ptr
         = &typeid(factory<Interface, Info, Param1>);
      FactoryMap::iterator it = 
      factories_.find(current_type_ptr);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1> * ret
     = new factory_container<Interface, Info, Param1>();
        factories_[current_type_ptr] = ret;
        return *ret;
      }
      else
      {
        return dynamic_cast<factory_container<Interface, Info, Param1> &>(*(it->second));
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
    std::list<factory<Interface, Info, Param1, Param2> > & get()
  {
      const std::type_info * current_type_ptr
         = &typeid(factory<Interface, Info, Param1, Param2>);
      FactoryMap::iterator it = 
      factories_.find(current_type_ptr);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2> * ret
     = new factory_container<Interface, Info, Param1, Param2>();
        factories_[current_type_ptr] = ret;
        return *ret;
      }
      else
      {
        return dynamic_cast<factory_container<Interface, Info, Param1, Param2> &>(*(it->second));
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
    std::list<factory<Interface, Info, Param1, Param2, Param3> > & get()
  {
      const std::type_info * current_type_ptr
         = &typeid(factory<Interface, Info, Param1, Param2, Param3>);
      FactoryMap::iterator it = 
      factories_.find(current_type_ptr);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3> * ret
     = new factory_container<Interface, Info, Param1, Param2, Param3>();
        factories_[current_type_ptr] = ret;
        return *ret;
      }
      else
      {
        return dynamic_cast<factory_container<Interface, Info, Param1, Param2, Param3> &>(*(it->second));
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
    std::list<factory<Interface, Info, Param1, Param2, Param3, Param4> > & get()
  {
      const std::type_info * current_type_ptr
         = &typeid(factory<Interface, Info, Param1, Param2, Param3, Param4>);
      FactoryMap::iterator it = 
      factories_.find(current_type_ptr);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3, Param4> * ret
     = new factory_container<Interface, Info, Param1, Param2, Param3, Param4>();
        factories_[current_type_ptr] = ret;
        return *ret;
      }
      else
      {
        return dynamic_cast<factory_container<Interface, Info, Param1, Param2, Param3, Param4> &>(*(it->second));
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
    std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5> > & get()
  {
      const std::type_info * current_type_ptr
         = &typeid(factory<Interface, Info, Param1, Param2, Param3, Param4, Param5>);
      FactoryMap::iterator it = 
      factories_.find(current_type_ptr);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5> * ret
     = new factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5>();
        factories_[current_type_ptr] = ret;
        return *ret;
      }
      else
      {
        return dynamic_cast<factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5> &>(*(it->second));
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
    std::list<factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> > & get()
  {
      const std::type_info * current_type_ptr
         = &typeid(factory<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6>);
      FactoryMap::iterator it = 
      factories_.find(current_type_ptr);
      
      if (it == factories_.end())
      {
        factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> * ret
     = new factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6>();
        factories_[current_type_ptr] = ret;
        return *ret;
      }
      else
      {
        return dynamic_cast<factory_container<Interface, Info, Param1, Param2, Param3, Param4, Param5, Param6> &>(*(it->second));
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


}}
#include <boost/extension/impl/typeinfo.hpp>
#endif
