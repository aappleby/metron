#include "metron_tools.h"

// We can instantiated templated classes as submodules.

class Submod {
public:

  void tick() {
    sub_reg = sub_reg + 1;
  }

  logic<8> sub_reg;
};

class Module {
public:

  logic<8> get_submod_reg() const {
    return submod.sub_reg;
  }

  void tock() {
    submod.tick();
  }

  Submod submod;
};
