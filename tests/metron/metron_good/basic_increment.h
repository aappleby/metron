#include "metron/metron_tools.h"

// Increment/decrement should be translated into equivalent Verilog, but they
// do _not_ return the old/new value.

class Module {
public:

  void tick() {
    my_reg1_++;
    ++my_reg2_;
    my_reg3_--;
    --my_reg4_;
  }

  int my_reg1_;
  int my_reg2_;
  int my_reg3_;
  int my_reg4_;
};
