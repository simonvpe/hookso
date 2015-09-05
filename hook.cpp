#include <iostream>
#include <string>
#include "hook.h"

using namespace std;

// Hook implementation for malloc. Just prints to cout when pre/post hooks
// are called.
static struct Hook_malloc : public Hook<Hook_malloc, void*(size_t)> {
  virtual void pre_hook(size_t) {
    cout << "malloc pre-hook called" << endl;
  }
  virtual void post_hook(void *p, size_t) {
    cout << "malloc post-hook called. Addres of allocated memory is " << p << endl;
  }
  static constexpr const char *function_name() { return "malloc"; }
} malloc_hook;
void *malloc(size_t size) { return malloc_hook(size); }

// Override rand to provide a sequence of numbers
static struct Hook_rand : public Hook<Hook_rand, int()> {
  int count;
  virtual int original_function() {
    int actual = fn();
    cout << "rand ovveride called. Substituting random number "
         << actual << " for " << count << endl;
    return count++;
  }
  static constexpr const char *function_name() { return "rand"; }

  Hook_rand() : count(0) {}
} rand_hook;
int rand(void) { return rand_hook(); }

// TODO: Re-implement a c++ class member
