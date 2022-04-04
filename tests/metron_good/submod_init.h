#include "metron_tools.h"

// Modules can call init() on their submodules.

class Submod {
public:

  void init() {
    sub_reg = 3;
  }

  logic<8> tock() {
    return sub_reg;
  }

private:
  logic<8> sub_reg;
};

class Module {
public:

  void init() {
    submod.init();
  }

private:

  Submod submod;
};
