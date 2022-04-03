#include "metron_tools.h"

// Writing a register multiple times in the same function is OK.

class Module {

  void tick() {
    logic<1> temp = reg;
    reg = 0;
    reg = 1;
  }

  logic<1> reg;
};
