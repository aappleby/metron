#include "metron_tools.h"

// All submod input ports have to be bound.

//X No input bound to component port submod.b

class Submod {
public:

  logic<1> tock_foo(logic<1> a) const {
    return !a;
  }

  logic<1> tock_bar(logic<1> b) const {
    return !b;
  }
};

class Module {
public:

  void tock() {
    submod.tock_foo(1);
  }

private:

  Submod submod;
};
