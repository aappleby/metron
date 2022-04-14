#include "metron_tools.h"

// Multiple tick methods that write the same reg are not allowed.

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
    reg1 = 1;
  }

  logic<1> reg1;
};
