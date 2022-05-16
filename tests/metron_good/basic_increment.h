#include "metron_tools.h"

// Increment/decrement should be translated into equivalent Verilog, but they
// do _not_ return the old/new value.

class Module {
public:

  int tock() {
    tick();
    return 0;
  }

private:

  void tick() {
    my_reg1++;
    ++my_reg2;
    my_reg3--;
    --my_reg4;
  }

  int my_reg1;
  int my_reg2;
  int my_reg3;
  int my_reg4;
};
