#include "metron_tools.h"

// A submod function can only produce one value per tock(). Trying to use it
// twice should be an error.

//X Writing field submod.tock_add_one.a changed state from FIELD_SIGNAL to FIELD_INVALID

class Submod {
public:

  logic<8> tock_add_one(logic<8> a) const {
    return a + 1;
  }
};


class Module {
public:

  void tock() {
    b = submod.tock_add_one(1);
    c = submod.tock_add_one(2);
  }

private:

  logic<8> b;
  logic<8> c;
  Submod submod;
};
