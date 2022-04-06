#include "metron_tools.h"

// If input ports get deduped, we have to somehow ensure that all bindings to
// the deduped port are the same...?

class Submod {
public:

  logic<8> thing1(logic<8> dedup) const {
    return dedup + 1;
  }

  logic<8> thing2(logic<8> dedup) const {
    return dedup + 2;
  }
};

class Module {
public:

  logic<8> tock() {
    logic<8> dedup1 = 10;
    logic<8> dedup2 = 20;

    return submod.thing1(dedup1) + submod.thing2(dedup2);
  }

  Submod submod;

};
