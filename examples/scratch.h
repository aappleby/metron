#include "metron/tools/metron_tools.h"

// Modules can contain other modules.

class Submod {
public:

  Submod(int width = 7) {
  }

  int tock() {
    tick();
    return 1;
  }

private:

  void tick() {
    sub_reg = sub_reg + 1;
  }

  logic<8> sub_reg;
};

class Module {
public:

  Module() : submod(2) {
  }

  void tock() {
    submod.tock();
  }

  Submod submod;
};
