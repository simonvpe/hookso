#pragma once

#include <type_traits>


extern "C" {
#  include <dlfcn.h>
}

// Make specializations of these traits to override default
// behaviour.
template<typename Impl_Type>
struct enable_prehook {
  static const bool value = true;
};

template<typename Impl_Type>
struct enable_posthook {
  static const bool value = true;
};

template<typename Impl_Type>
struct enable_func_override {
  static const bool value = true;
};

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

  public:
    Hook() {
      this->fn = reinterpret_cast<Function_T>(dlsym(RTLD_NEXT, Impl_Type::function_name())); 
    }
    virtual ~Hook() {}
    Hook(const Hook &) = delete;
    Hook &operator=(const Hook &) = delete;

    Return_Type operator()(Args & ... args) const {
      prehook(args...);
      Return_Type ret = func_override(args...);
      posthook(ret);
      return ret;
    }

    // *************************************************************************************
    // void prehook(Args & ... args)
    template<typename T = Impl_Type>
      inline typename enable_if< !enable_prehook<T>::value, void >::type
      prehook(Args & ... args) const {
      // Do nothing
    }
    template<typename T = Impl_Type>
      inline typename enable_if< enable_prehook<T>::value, void >::type
      prehook(Args & ... args) const {
      reinterpret_cast<const Impl_Type*>(this)->prehook(args...);
    }
    // *************************************************************************************
    // void posthook(Return_Type &ret)
    template<typename T = Impl_Type>
      inline typename enable_if< !enable_posthook<T>::value, void >::type
      posthook(Return_Type &ret) const {
      // Do nothing
    }
    template<typename T = Impl_Type>
      inline typename enable_if< enable_posthook<T>::value, void >::type
      posthook(Return_Type &ret) const {
      reinterpret_cast<const Impl_Type*>(this)->posthook(ret);
    }
    // *************************************************************************************
    // Return_Type func_override(Args...)
    template<typename T = Impl_Type>
      inline typename enable_if< !enable_func_override<T>::value, Return_Type >::type
      func_override(Args & ... args) const {
      return const_cast<This_T*>(this)->fn(args...); 
    }
    template<typename T = Impl_Type>
      inline typename enable_if< enable_func_override<T>::value, Return_Type >::type
      func_override(Args & ... args) const {
      return reinterpret_cast<Impl_Type*>(const_cast<This_T*>(this))->func_override(args...);
    }
    // *************************************************************************************
  };
} // namespace hooksope
