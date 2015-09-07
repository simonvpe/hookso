#include <iostream>
#include <string>
#include "hook.h"

using namespace std;
using namespace hookso;

BEGIN_HOOK(void*, malloc, size_t bla)
{
  static const bool enable_prehook = true;
  static const bool enable_posthook = true;
  static const bool enable_implementation = false;

  inline void prehook(size_t) const {
    cout << "malloc pre-hook called" << endl;
  }
  inline void posthook(void *p) const {
    cout << "malloc post-hook called. Addres of allocated memory is " << p << endl;
  }
}
END_HOOK(void*, malloc, size_t bla)
void *malloc(size_t size) { return hook_malloc(size); }

// Override rand to provide a sequence of numbers
BEGIN_HOOK(int, rand)
{
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
 
  Hook_rand() : count(0) {}
}
END_HOOK(int, rand)
int rand(void) { return hook_rand(); }

// TODO: Re-implement a c++ class member
