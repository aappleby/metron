#include "metron/metron_tools.h"

// All the combos of tasks/funcs should work from ticks and tocks.

// Yosys bug - fails to parse if we pass constants between the functions
// https://github.com/YosysHQ/yosys/issues/3327

class Module {
public:

  logic<8> my_sig;

  void tock(int z) {
    public_task(public_func(z));
    tick(z);
  }

  void public_task(logic<8> x) {
    my_sig = x + 7;
  }

  logic<8> public_func(logic<8> x) {
    return my_reg1_ + private_func(x);
  }

private:

  void tick(int w) {
    private_task(private_func(w));
    my_reg2_ = my_reg2_ + 1;
  }

  void private_task(logic<8> x) {
    my_reg1_ = my_reg1_ + private_func(x);
  }

  logic<8> private_func(logic<8> y) {
    return my_reg1_ + y;
  }

  logic<8> my_reg1_;
  logic<8> my_reg2_;
};
