#include "metron_tools.h"

// Private non-const methods should turn into SV tasks.

class Module {
public:

  void tick() {
    some_task();
  }

private:

  void some_task() {
    my_reg = my_reg + 3;
  }

  logic<8> my_reg;
};
