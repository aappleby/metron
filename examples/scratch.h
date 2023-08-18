#include "metron/tools/metron_tools.h"

// Increment/decrement should be translated into equivalent Verilog, but they
// do _not_ return the old/new value.

class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick(int a, int b, int c = 12) {
    my_reg1++;
    ++my_reg2;
    my_reg3--;
    --my_reg4;
  }

  const char* blarp = "hello world";

  int my_reg1;
  int my_reg2;
  int my_reg3;
  int my_reg4;
};
