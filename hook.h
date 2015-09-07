#pragma once

#include <type_traits>


extern "C" {
#  include <dlfcn.h>
}

// Check which features are enabled. If the class defines a type called
// enable_##X it picks that value, otherwise it picks default_value
#define MAKE_ENABLE_CHECK(X, default_value)				\
  template<typename Impl_Type>						\
  struct enable_##X {							\
									\
  template<typename U>							\
  struct has_##X {							\
  typedef char is_match;						\
  typedef long no_match;	 					\
  template<typename C> static is_match test( decltype(&C::enable_##X) );\
  template<typename C> static no_match test(...);			\
  enum { value = sizeof(test<U>(0)) == sizeof(char) };			\
  };									\
									\
  template<typename U, bool select> struct get_##X;			\
  template<typename U> struct get_##X<U,false> { enum { value = default_value }; }; \
  template<typename U> struct get_##X<U,true>  { enum { value = U::enable_##X }; }; \
									\
  static const bool value = get_##X<Impl_Type, has_##X<Impl_Type>::value>::value; \
  };									\

// We want to be able to check for prehook, posthook, and implementation in order
// to pick whether to dispatch function calls during compile time.
MAKE_ENABLE_CHECK(prehook, true)
MAKE_ENABLE_CHECK(posthook, true)
MAKE_ENABLE_CHECK(implementation, true)

namespace hookso {
  using namespace std;

  // I order to use function signature (i.e. int &(bool, int) ) as
  // template parameter.
  template<typename Impl_Type, typename Signature>
    class Hook;

  template<typename Impl_Type, typename Return_Type, typename... Args>
    class Hook<Impl_Type, Return_Type(Args...)> {

  protected:
    using This_T = Hook<Impl_Type, Return_Type(Args...)>;
    using Function_T = Return_Type (*)(Args...);
    Function_T fn;

    // Constructor stores a function pointer to the actual function we are overriding
    Hook() {
      this->fn = reinterpret_cast<Function_T>(dlsym(RTLD_NEXT, Impl_Type::function_name())); 
    }
    virtual ~Hook() {}

    // Disable copying
    Hook(const Hook &) = delete;
    Hook &operator=(const Hook &) = delete;

  public:
    
    // Call procedure, in the function override you must call this function with the actual
    // arguments.
    Return_Type operator()(Args & ... args) const {
      _prehook(args...);
      Return_Type ret = _impl(args...);
      _posthook(ret);
      return ret;
    }

    // *************************************************************************************
    // void prehook(Args & ... args)
    // *************************************************************************************
    template<typename T = Impl_Type>
      inline typename enable_if< !enable_prehook<T>::value, void >::type
      _prehook(Args & ... args) const {
      // Do nothing if enable_prehook != true
    }
    template<typename T = Impl_Type>
      inline typename enable_if< enable_prehook<T>::value, void >::type
      _prehook(Args & ... args) const {
      // Call prehook if enable_prehook == true
      reinterpret_cast<const Impl_Type*>(this)->prehook(args...);
    }
    // *************************************************************************************
    // void posthook(Return_Type &ret)
    // *************************************************************************************
    template<typename T = Impl_Type>
      inline typename enable_if< !enable_posthook<T>::value, void >::type
      _posthook(Return_Type &ret) const {
      // Do nothing if enable_posthook != true
    }
    template<typename T = Impl_Type>
      inline typename enable_if< enable_posthook<T>::value, void >::type
      _posthook(Return_Type &ret) const {
      // Call posthook if enable_posthook == true
      reinterpret_cast<const Impl_Type*>(this)->posthook(ret);
    }
    // *************************************************************************************
    // Return_Type func_override(Args...)
    // *************************************************************************************
    template<typename T = Impl_Type>
      inline typename enable_if< !enable_implementation<T>::value, Return_Type >::type
      _impl(Args & ... args) const {
      // Call actual function if enable_implementation != true
      return const_cast<This_T*>(this)->fn(args...); 
    }
    template<typename T = Impl_Type>
      inline typename enable_if< enable_implementation<T>::value, Return_Type >::type
      _impl(Args & ... args) const {
      // Call user implementation if enable_implementation == true
      return reinterpret_cast<Impl_Type*>(const_cast<This_T*>(this))->implementation(args...);
    }
    // *************************************************************************************
  };
} // namespace hooksope
