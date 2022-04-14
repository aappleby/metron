#include "metron_tools.h"

// Registers are "locked" when the function that writes to them exits and
// cannot be written in another function.

class Module {
public:

  void tock() {
    tick1();
    tick2();
  }

private:

  void tick1() {
    reg1 = 0;
  }

  void tick2() {
    reg1 = 0;
    reg2 = 1;
  }

  logic<1> reg1;
  logic<1> reg2;
};
