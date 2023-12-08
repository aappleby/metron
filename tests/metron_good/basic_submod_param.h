#include "metron/metron_tools.h"

// We can instantiated templated classes as submodules.

template<int SOME_CONSTANT = 6>
class Submod {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    sub_reg_ = sub_reg_ + SOME_CONSTANT;
  }

  logic<8> sub_reg_;
};

class Module {
public:

  void tock() {
    submod_.tock();
  }

  Submod<99> submod_;
};
