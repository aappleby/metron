#include "metron/metron_tools.h"

// Writing a register multiple times in the same function is OK.

class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    logic<1> temp = my_reg_;
    my_reg_ = 0;
    my_reg_ = 1;
  }

  logic<1> my_reg_;
};
