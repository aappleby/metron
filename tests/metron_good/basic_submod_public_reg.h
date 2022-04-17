#include "metron_tools.h"

// We can instantiated templated classes as submodules.

class Submod {
public:

  void tock() {
    tick();
  }

  logic<8> sub_reg;

private:

  void tick() {
    sub_reg = sub_reg + 1;
  }

};

class Module {
public:

  logic<8> tock_get_submod_reg() const {
    return submod.sub_reg;
  }

  void tock() {
    submod.tock();
  }

  Submod submod;
};
