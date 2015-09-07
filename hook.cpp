#include <iostream>
#include <string>
#include "hook.h"

using namespace std;
using namespace hookso;

// Hook implementation for malloc. Just prints to cout when pre/post hooks
// are called.
static struct Hook_malloc : public Hook<Hook_malloc, void*(size_t)> {
  // Config
  static const bool enable_prehook = true;
  static const bool enable_posthook = false;
  static const bool enable_implementation = false;

  inline void prehook(size_t) const {
    cout << "malloc pre-hook called" << endl;
  }
  inline void posthook(void *p) const {
    cout << "malloc post-hook called. Addres of allocated memory is " << p << endl;
  }
  static constexpr const char *function_name() { return "malloc"; }
} malloc_hook;
void *malloc(size_t size) { return malloc_hook(size); }

// Override rand to provide a sequence of numbers
static struct Hook_rand : public Hook<Hook_rand, int()> {

  // Config
  static const bool enable_prehook = false;
  static const bool enable_posthook = false;
  static const bool enable_implementation = true;

  int count;
  inline int implementation() {
    int actual = fn();
    cout << "rand ovveride called. Substituting random number "
         << actual << " for " << count << endl;
    return count++;
  }
  static const char *function_name() { return "rand"; }

  Hook_rand() : count(0) {}
} rand_hook;
int rand(void) { return rand_hook(); }

// TODO: Re-implement a c++ class member
