#include "metron/metron_tools.h"

// Private non-const methods should turn into SV tasks.

class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    my_reg_ = my_reg_ + my_reg2_ + 3;
    some_task2();
  }

  void some_task2() {
    my_reg2_ = my_reg2_ + 3;
  }

  logic<8> my_reg_;
  logic<8> my_reg2_;
};
