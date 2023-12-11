#include "metron/metron_tools.h"

// We can instantiated templated classes as submodules.

class Submod {
public:

  void tock() {
    tick();
  }

  logic<8> sub_reg_;

private:

  void tick() {
    sub_reg_ = sub_reg_ + 1;
  }

};

class Module {
public:

  logic<8> get_submodreg() const {
    return submod.sub_reg_;
  }

  void tock() {
    submod.tock();
  }

  Submod submod;
};
