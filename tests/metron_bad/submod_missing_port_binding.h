#include "metron_tools.h"

// All submod input ports have to be bound.
// FIXME - Maybe only tick/tock ports have to be bound?

class Submod {
public:

  logic<1> foo(logic<1> a) const {
    return !a;
  }

  logic<1> bar(logic<1> b) const {
    return !b;
  }
};

class Module {
public:

  void tick() {
    submod.foo(1);
  }

private:
  Submod submod;
};
