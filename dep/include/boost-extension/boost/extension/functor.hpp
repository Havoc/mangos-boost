/* (C) Copyright Jeremy Pack 2007
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_EXTENSION_FUNCTOR_HPP
#define BOOST_EXTENSION_FUNCTOR_HPP

#include <boost/extension/impl/linked_library.hpp>

#ifdef BOOST_EXTENSIONS_USE_PP

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition.hpp>

#ifndef BOOST_EXTENSIONS_MAX_FUNCTOR_PARAMS
#define BOOST_EXTENSIONS_MAX_FUNCTOR_PARAMS 6
#endif

/// functor template specialization macro.
#define BOOST_EXTENSIONS_FUNCTOR_CLASS(Z, N, _) \
    template<class ReturnValue BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, class Param) > \
    class functor<ReturnValue BOOST_PP_COMMA_IF(N) BOOST_PP_ENUM_PARAMS(N, Param) > { \
    private: \
        typedef ReturnValue (*FunctionType)(BOOST_PP_ENUM_PARAMS(N, Param)); \
        FunctionType func_; \
    public: \
        bool is_valid() const {return func_ != 0;} \
        functor(FunctionType func) \
            : func_(func) {} \
        functor(generic_function_ptr func) \
            : func_(FunctionType(func)) {} \
        ReturnValue operator()(BOOST_PP_ENUM_BINARY_PARAMS(N, Param, p)) { \
            return func_(BOOST_PP_ENUM_PARAMS(N, p)); \
        } \
    }; \
/**/

#endif // ifdef BOOST_EXTENSIONS_USE_PP


namespace boost { namespace extensions {

using boost::extensions::impl::generic_function_ptr;

#ifdef BOOST_EXTENSIONS_USE_PP

/// Declaration of functor class template.
template <class ReturnValue,
    BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(BOOST_PP_INC(BOOST_EXTENSIONS_MAX_FUNCTOR_PARAMS), class Param, void)>
    class functor;

/// Functor template specializations.
BOOST_PP_REPEAT(BOOST_PP_INC(BOOST_EXTENSIONS_MAX_FUNCTOR_PARAMS), BOOST_EXTENSIONS_FUNCTOR_CLASS, _)

#undef BOOST_EXTENSIONS_FUNCTOR_CLASS
#else

template <class ReturnValue, class Param1 = void, class Param2 = void, class Param3 = void, 
class Param4 = void, class Param5 = void, class Param6 = void>
class functor {
private:
    typedef ReturnValue (*FunctionType)(Param1, Param2, Param3, Param4, Param5, Param6);
    FunctionType func_;

public:
    bool is_valid() const {return func_ != 0;}

    functor(FunctionType func)
        : func_(func)
    {}

    functor(generic_function_ptr func)
        : func_(FunctionType(func))
    {}

    ReturnValue operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)
    {
        return func_(p1, p2, p3, p4, p5, p6);
    }
};

template <class ReturnValue, class Param1, class Param2, class Param3, class Param4, class Param5>
class functor<ReturnValue, Param1, Param2, Param3, Param4, Param5>
{
private:
    typedef ReturnValue (*FunctionType)(Param1, Param2, Param3, Param4, Param5);
    FunctionType func_;

public:
    bool is_valid() const {return func_ != 0;}

    functor(FunctionType func)
        :func_(func)
    {}
    functor(generic_function_ptr func)
        :func_(FunctionType(func))
    {}
    ReturnValue operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)
    {
        return func_(p1, p2, p3, p4, p5);
    }
};

template <class ReturnValue, class Param1, class Param2, class Param3, class Param4>
class functor<ReturnValue, Param1, Param2, Param3, Param4>
{
private:
    typedef ReturnValue (*FunctionType)(Param1, Param2, Param3, Param4);
    FunctionType func_;

public:
    bool is_valid() const {return func_ != 0;}

    functor(FunctionType func)
        :func_(func)
    {}

    functor(generic_function_ptr func)
        :func_(FunctionType(func))
    {}

    ReturnValue operator()(Param1 p1, Param2 p2, Param3 p3, Param4 p4)
    {
        return func_(p1, p2, p3, p4);
    }
};

template <class ReturnValue, class Param1, class Param2, class Param3>
class functor<ReturnValue, Param1, Param2, Param3>
{
private:
    typedef ReturnValue (*FunctionType)(Param1, Param2, Param3);
    FunctionType func_;

public:
    bool is_valid() const {return func_ != 0;}

    functor(FunctionType func)
        :func_(func)
    {}
    functor(generic_function_ptr func)
        :func_(FunctionType(func))
    {}
    ReturnValue operator()(Param1 p1, Param2 p2, Param3 p3)
    {
        return func_(p1, p2, p3);
    }
};

template <class ReturnValue, class Param1, class Param2>
class functor<ReturnValue, Param1, Param2>
{
private:
    typedef ReturnValue (*FunctionType)(Param1, Param2);
    FunctionType func_;

public:
    bool is_valid() const {return func_ != 0;}

    functor(FunctionType func)
        :func_(func)
    {}
    functor(generic_function_ptr func)
        :func_(FunctionType(func))
    {}
    ReturnValue operator()(Param1 p1, Param2 p2)
    {
        return func_(p1, p2);
    }
};

template <class ReturnValue, class Param1>
class functor<ReturnValue, Param1>
{
private:
    typedef ReturnValue (*FunctionType)(Param1);
    FunctionType func_;

public:
    bool is_valid() const {return func_ != 0;}
    functor(FunctionType func)
        :func_(func)
    {}
    functor(generic_function_ptr func)
        :func_(FunctionType(func))
    {}
    ReturnValue operator()(Param1 p1)
    {
        return func_(p1);
    }
};

template <class ReturnValue>
class functor<ReturnValue>
{
private:
    typedef ReturnValue (*FunctionType)();
    FunctionType func_;

public:
    bool is_valid() const {return func_ != 0;}
    functor(FunctionType func)
        :func_(func)
    {}
    functor(generic_function_ptr func)
        :func_(FunctionType(func))
    {}
    ReturnValue operator()()
    {
        return func_();
    }
};

#endif // ifdef BOOST_EXTENSIONS_USE_PP

}} // ns: boost::extensions

#endif // BOOST_EXTENSION_FUNCTOR_HPP
