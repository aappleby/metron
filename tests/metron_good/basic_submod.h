#include "metron_tools.h"

// Modules can contain other modules.

class Submod {
public:

  void tick() {
    sub_reg = sub_reg + 1;
  }

  logic<8> sub_reg;
};

class Module {
public:

  void tock() {
    submod.tick();
  }

  Submod submod;
};
