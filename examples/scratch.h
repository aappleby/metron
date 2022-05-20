#include "metron_tools.h"

// All the combos of tasks/funcs should work from ticks and tocks.

// Yosys bug - fails to parse if we pass constants between the functions
// https://github.com/YosysHQ/yosys/issues/3327

class Module {
public:

  void tock() {
    tick(1);
  }

  void tick(int w) {
    my_reg2 = my_reg2 + w;
    private_task(w);
  }

  void private_task(logic<8> x) {
    my_reg1 = my_reg1 + x;
  }

  logic<8> my_reg1;
  logic<8> my_reg2;
};
