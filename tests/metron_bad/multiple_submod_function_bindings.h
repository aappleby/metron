#include "metron_tools.h"

// A submod function can only produce one value per tock(). Trying to use it
// twice should be an error.

// X Could not categorize all methods

class Submod {
 public:
  logic<8> tock_add_one(logic<8> a) const { return a + 1; }
};

class Module {
 public:
  void tock() {
    b = submod.tock_add_one(b);
    c = submod.tock_add_one(c);
  }

 private:
  logic<8> b;
  logic<8> c;
  Submod submod;
};
