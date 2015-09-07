#include <iostream>
#include <string>
#include "hook.h"

using namespace std;
using namespace hookso;

// Hook implementation for malloc. Just prints to cout when pre/post hooks
// are called.
static struct Hook_malloc : public Hook<Hook_malloc, void*(size_t)> {
  inline void prehook(size_t) const {
    cout << "malloc pre-hook called" << endl;
  }
  inline void posthook(void *p) const {
    cout << "malloc post-hook called. Addres of allocated memory is " << p << endl;
  }
  static constexpr const char *function_name() { return "malloc"; }
} malloc_hook;
template<> struct enable_prehook<Hook_malloc>       { static const bool value = true;  };
template<> struct enable_posthook<Hook_malloc>      { static const bool value = true;  };
template<> struct enable_func_override<Hook_malloc> { static const bool value = false; };
void *malloc(size_t size) { return malloc_hook(size); }

// Override rand to provide a sequence of numbers
//HOOK_BEGIN(Hook_rand, int())
static struct Hook_rand : public Hook<Hook_rand, int()> {
  int count;
  inline int func_override() {
    int actual = fn();
    cout << "rand ovveride called. Substituting random number "
         << actual << " for " << count << endl;
    return count++;
  }
  static const char *function_name() { return "rand"; }

  Hook_rand() : count(0) {}
//HOOK_END(Hook_rand)
} rand_hook;
template<> struct enable_prehook<Hook_rand>       { static const bool value = false; };
template<> struct enable_posthook<Hook_rand>      { static const bool value = false; };
template<> struct enable_func_override<Hook_rand> { static const bool value = true;  };
int rand(void) { return rand_hook(); }

// TODO: Re-implement a c++ class member
