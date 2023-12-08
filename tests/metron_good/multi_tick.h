#include "metron/metron_tools.h"

// Multiple tick methods are OK as long as they're all called by tock()

class Module {
public:

  logic<8> tock() {
    logic<8> result = my_reg1_ + my_reg2_;
    tick1();
    tick2();
    return result;
  }

private:

  void tick1() {
    my_reg1_ = 0;
  }

  void tick2() {
    my_reg2_ = 1;
  }

  logic<8> my_reg1_;
  logic<8> my_reg2_;
};
