#include "metron_tools.h"

// Private non-const methods should turn into SV tasks.

class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    some_task();
  }

  void some_task() {
    my_reg = my_reg + my_reg2 + 3;
    some_task2();
  }

  void some_task2() {
    my_reg2 = my_reg2 + 3;
  }

  logic<8> my_reg;
  logic<8> my_reg2;
};
