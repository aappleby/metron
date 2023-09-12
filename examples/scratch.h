#include "metron/tools/metron_tools.h"

// Modules can contain other modules.

template<int width = 7>
class Submod {
public:

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

  void tock() {
    submod.tock();
  }

  Submod<2> submod;
};
