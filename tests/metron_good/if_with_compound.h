#include "metron/metron_tools.h"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

class Submod {
public:
  void tock(logic<8> arg) {
    tick(arg);
  }
private:
  void tick(logic<8> arg) {
    my_reg_ = my_reg_ + arg;
  }

  logic<8> my_reg_;
};


class Module {
public:

  void tock() {
    if (1) {
      submod_.tock(72);
    }
    else {
      submod_.tock(36);
    }
  }

  Submod submod_;
};
