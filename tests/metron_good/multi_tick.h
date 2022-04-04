#include "metron_tools.h"

// Multiple tick methods are OK as long as they're all called by tock()

class Module {
public:

  void tock() {
    tick1();
    tick2();
  }

private:

  void tick1() {
    my_reg1 = 0;
  }

  void tick2() {
    my_reg2 = 1;
  }

  logic<1> my_reg1;
  logic<1> my_reg2;
};
