/* (C) Copyright Jeremy Pack 2007
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef BOOST_EXTENSION_FACTORY_HPP
#define BOOST_EXTENSION_FACTORY_HPP
#include <string>
namespace boost{namespace extensions{
  template <class Interface, class Info, class Param1 = void, class Param2 = void, class Param3 = void, class Param4 = void, class Param5 = void, class Param6 = void>
class factory
{
private:
  class generic_factory_function
  {
  public:
    virtual ~generic_factory_function(){}
    virtual Interface * operator()(Param1, Param2, Param3, Param4, Param5, Param6) = 0;
    virtual generic_factory_function * copy() const = 0;
  };
  template <class T>
  class factory_function : public generic_factory_function
  {
  public:
    virtual ~factory_function(){}
    virtual Interface * operator()
      (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)
    {return new T(p1, p2, p3, p4, p5, p6);}
    virtual generic_factory_function * copy() const {return new factory_function<T>;}
  };
  std::auto_ptr<generic_factory_function> factory_func_ptr_;
  Info info_;
public:
  template <class Actual>
    void set_type_special(Actual *){factory_func_ptr_.reset(new factory_function<Actual>());}
  template <class Actual>
    void set_type(){factory_func_ptr_ = new factory_function<Actual>();}
  factory(Info info)
    :factory_func_ptr_(0),
    info_(info)
  {}
  factory(const factory & first)
    :factory_func_ptr_(first.factory_func_ptr_->copy()),
    info_(first.info_)
                       {}
  Interface * operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)
    {return create(p1, p2, p3, p4, p5, p6);}
  Interface * create(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6){return (*factory_func_ptr_)
  (p1, p2, p3, p4, p5, p6);}
  Info & get_info(){return info_;}
};
template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4, class Param5>
class factory<Interface, Info, Param1, Param2, Param3, Param4, Param5>
{
private:
  class generic_factory_function
  {
  public:
    virtual ~generic_factory_function(){}
    virtual Interface * operator()(Param1, Param2, Param3, Param4, Param5) = 0;
    virtual generic_factory_function * copy() const = 0;
  };
  template <class T>
  class factory_function : public generic_factory_function
  {
  public:
    virtual ~factory_function(){}
    virtual Interface * operator()
      (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)
    {return new T(p1, p2, p3, p4, p5);}
    virtual generic_factory_function * copy() const {return new factory_function<T>;}
  };
  std::auto_ptr<generic_factory_function> factory_func_ptr_;
  Info info_;
public:
  template <class Actual>
    void set_type_special(Actual *){factory_func_ptr_.reset(new factory_function<Actual>());}
  template <class Actual>
    void set_type(){factory_func_ptr_ = new factory_function<Actual>();}
  factory(Info info)
    :factory_func_ptr_(0),
    info_(info)
  {}
  factory(const factory & first)
    :factory_func_ptr_(first.factory_func_ptr_->copy()),
    info_(first.info_)
                       {}
  Interface * operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)
    {return create(p1, p2, p3, p4, p5);}
  Interface * create(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5){return (*factory_func_ptr_)
  (p1, p2, p3, p4, p5);}
  Info & get_info(){return info_;}
};
template <class Interface, class Info, class Param1, class Param2, class Param3, class Param4>
class factory<Interface, Info, Param1, Param2, Param3, Param4>
{
private:
  class generic_factory_function
  {
  public:
    virtual ~generic_factory_function(){}
    virtual Interface * operator()(Param1, Param2, Param3, Param4) = 0;
    virtual generic_factory_function * copy() const = 0;
  };
  template <class T>
  class factory_function : public generic_factory_function
  {
  public:
    virtual ~factory_function(){}
    virtual Interface * operator()
      (Param1 p1, Param2 p2, Param3 p3, Param4 p4)
    {return new T(p1, p2, p3, p4);}
    virtual generic_factory_function * copy() const {return new factory_function<T>;}
  };
  std::auto_ptr<generic_factory_function> factory_func_ptr_;
  Info info_;
public:
  template <class Actual>
    void set_type_special(Actual *){factory_func_ptr_.reset(new factory_function<Actual>());}
  template <class Actual>
    void set_type(){factory_func_ptr_ = new factory_function<Actual>();}
  factory(Info info)
    :factory_func_ptr_(0),
    info_(info)
  {}
  factory(const factory & first)
    :factory_func_ptr_(first.factory_func_ptr_->copy()),
    info_(first.info_)
                       {}
  Interface * operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4)
    {return create(p1, p2, p3, p4);}
  Interface * create(Param1 p1, Param2 p2, Param3 p3, Param4 p4){return (*factory_func_ptr_)
  (p1, p2, p3, p4);}
  Info & get_info(){return info_;}
};
template <class Interface, class Info, class Param1, class Param2, class Param3>
class factory<Interface, Info, Param1, Param2, Param3>
{
private:
  class generic_factory_function
  {
  public:
    virtual ~generic_factory_function(){}
    virtual Interface * operator()(Param1, Param2, Param3) = 0;
    virtual generic_factory_function * copy() const = 0;
  };
  template <class T>
  class factory_function : public generic_factory_function
  {
  public:
    virtual ~factory_function(){}
    virtual Interface * operator()
      (Param1 p1, Param2 p2, Param3 p3)
    {return new T(p1, p2, p3);}
    virtual generic_factory_function * copy() const {return new factory_function<T>;}
  };
  std::auto_ptr<generic_factory_function> factory_func_ptr_;
  Info info_;
public:
  template <class Actual>
    void set_type_special(Actual *){factory_func_ptr_.reset(new factory_function<Actual>());}
  template <class Actual>
    void set_type(){factory_func_ptr_ = new factory_function<Actual>();}
  factory(Info info)
    :factory_func_ptr_(0),
    info_(info)
  {}
  factory(const factory & first)
    :factory_func_ptr_(first.factory_func_ptr_->copy()),
    info_(first.info_)
                       {}
  Interface * operator()(Param1 p1, Param2 p2, Param3 p3)
    {return create(p1, p2, p3);}
  Interface * create(Param1 p1, Param2 p2, Param3 p3){return (*factory_func_ptr_)
  (p1, p2, p3);}
  Info & get_info(){return info_;}
};
template <class Interface, class Info, class Param1, class Param2>
class factory<Interface, Info, Param1, Param2>
{
private:
  class generic_factory_function
  {
  public:
    virtual ~generic_factory_function(){}
    virtual Interface * operator()(Param1, Param2) = 0;
    virtual generic_factory_function * copy() const = 0;
  };
  template <class T>
  class factory_function : public generic_factory_function
  {
  public:
    virtual ~factory_function(){}
    virtual Interface * operator()
      (Param1 p1, Param2 p2)
    {return new T(p1, p2);}
    virtual generic_factory_function * copy() const {return new factory_function<T>;}
  };
  std::auto_ptr<generic_factory_function> factory_func_ptr_;
  Info info_;
public:
  template <class Actual>
    void set_type_special(Actual *){factory_func_ptr_.reset(new factory_function<Actual>());}
  template <class Actual>
    void set_type(){factory_func_ptr_ = new factory_function<Actual>();}
  factory(Info info)
    :factory_func_ptr_(0),
    info_(info)
  {}
  factory(const factory & first)
    :factory_func_ptr_(first.factory_func_ptr_->copy()),
    info_(first.info_)
                       {}
  Interface * operator()(Param1 p1, Param2 p2)
    {return create(p1, p2);}
  Interface * create(Param1 p1, Param2 p2){return (*factory_func_ptr_)
  (p1, p2);}
  Info & get_info(){return info_;}
};
template <class Interface, class Info, class Param1>
class factory<Interface, Info, Param1>
{
private:
  class generic_factory_function
  {
  public:
    virtual ~generic_factory_function(){}
    virtual Interface * operator()(Param1) = 0;
    virtual generic_factory_function * copy() const = 0;
  };
  template <class T>
  class factory_function : public generic_factory_function
  {
  public:
    virtual ~factory_function(){}
    virtual Interface * operator()
      (Param1 p1)
    {return new T(p1);}
    virtual generic_factory_function * copy() const {return new factory_function<T>;}
  };
  std::auto_ptr<generic_factory_function> factory_func_ptr_;
  Info info_;
public:
  template <class Actual>
    void set_type_special(Actual *){factory_func_ptr_.reset(new factory_function<Actual>());}
  template <class Actual>
    void set_type(){factory_func_ptr_ = new factory_function<Actual>();}
  factory(Info info)
    :factory_func_ptr_(0),
    info_(info)
  {}
  factory(const factory & first)
    :factory_func_ptr_(first.factory_func_ptr_->copy()),
    info_(first.info_)
                       {}
  Interface * operator()(Param1 p1)
    {return create(p1);}
  Interface * create(Param1 p1){return (*factory_func_ptr_)
  (p1);}
  Info & get_info(){return info_;}
};
template <class Interface, class Info>
class factory<Interface, Info>
{
private:
  class generic_factory_function
  {
  public:
    virtual ~generic_factory_function(){}
    virtual Interface * operator()() = 0;
    virtual generic_factory_function * copy() const = 0;
  };
  template <class T>
  class factory_function : public generic_factory_function
  {
  public:
    virtual ~factory_function(){}
    virtual Interface * operator()
      ()
    {return new T();}
    virtual generic_factory_function * copy() const {return new factory_function<T>;}
  };
  std::auto_ptr<generic_factory_function> factory_func_ptr_;
  Info info_;
public:
  template <class Actual>
    void set_type_special(Actual *){factory_func_ptr_.reset(new factory_function<Actual>());}
  template <class Actual>
    void set_type(){factory_func_ptr_ = new factory_function<Actual>();}
  factory(Info info)
    :factory_func_ptr_(0),
    info_(info)
  {}
  factory(const factory & first)
    :factory_func_ptr_(first.factory_func_ptr_->copy()),
    info_(first.info_)
                       {}
  Interface * operator()()
    {return create();}
  Interface * create(){return (*factory_func_ptr_)
  ();}
  Info & get_info(){return info_;}
};
}}
#endif
