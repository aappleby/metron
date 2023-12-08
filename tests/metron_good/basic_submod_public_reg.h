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

  logic<8> get_submod_reg() const {
    return submod_.sub_reg_;
  }

  void tock() {
    submod_.tock();
  }

  Submod submod_;
};
