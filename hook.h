#pragma once

#include <iostream>
using namespace std;

extern "C" {
#  include <dlfcn.h>
}

// Hook
// When statically instantiated in a shared object, it overrides
// a selected function with a user supplied implementation. Can
// also be used for doing pre/post processing of data when a
// specific function is called.

// Template arguments:
// * Impl_Type   - The implementation type, i.e. "Curiously Recurring Template".
// * Return_Type - The return type of the target function.
// * Args        - The arguments of the target function. It is currently not possible
//                 to override function with variable arguments (i.e. printf).

// Required member implementations:
// * The implementation must provide a function "static const char *name()" that
//   returns the name of the function to override.

// Virtual functions:
// * pre_hook          - Override if you wish to do preprocessing of the arguments
//                       prior to the call to the target function. 
// * post_hook         - Override if you wish to do postprocessing of the arguments
//                       after the call to the target function.
// * original_function - Override if you wish to intercept the functionality of the
//                       target function. The default implementation is to call
//                       "fn", a function pointer to the overridden function.

// In addition to providing an implementation one must implement the target function
// and call operator() on the Hook implementation instance.
//
// Please see examples in main.c

template<typename Impl_Type, typename Signature>
class Hook;

template<typename Impl_Type, typename Return_Type, typename... Args>
class Hook<Impl_Type, Return_Type(Args...)> {

protected:
  using Function_T = Return_Type (*)(Args...);
  Function_T fn;

public:
  Hook() {
    this->fn = reinterpret_cast<Function_T>(dlsym(RTLD_NEXT, Impl_Type::function_name())); 
  }
  virtual ~Hook() {}

  auto operator()(Args... args) -> Return_Type {
    // The reinterpret_casts hopefully makes sure to skip the vtable
    // lookups. Not sure about this though.
    Return_Type ret;
    reinterpret_cast<Impl_Type*>(this)->pre_hook(args...);
    ret = original_function(args...);
    reinterpret_cast<Impl_Type*>(this)->post_hook(ret, args...);
    return ret;
  }
  
  inline virtual void pre_hook(Args... args) {};
  inline virtual void post_hook(Return_Type ret, Args... args) {};
  inline virtual Return_Type original_function(Args... args) {
    return this->fn(args...); 
  }
};

