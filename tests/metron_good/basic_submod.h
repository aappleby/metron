#include "metron/tools/metron_tools.h"

// Modules can contain other modules.

class Submod {
public:

  void tock(int x) {
    tick(x);
  }

private:

  void tick(int x) {
    sub_reg = sub_reg + x;
  }

  logic<8> sub_reg;
};

class Module {
public:

  void tock(int x) {
    submod.tock(x);
  }

  Submod submod;
};
