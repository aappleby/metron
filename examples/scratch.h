#include "metron_tools.h"

// All the combos of tasks/funcs should work from ticks and tocks.

// Yosys bug - fails to parse if we pass constants between the functions
// https://github.com/YosysHQ/yosys/issues/3327

class Module {
public:

  logic<8> my_sig;

  void tock(int z) {
    logic<8> dummy = public_task(public_func(z));
  }

  logic<8> public_task(logic<8> x) {
    my_sig = x + 7;
    return 0;
  }

  logic<8> public_func(logic<8> x) {
    return my_reg1 + 1;
  }

  logic<8> my_reg1;
};
