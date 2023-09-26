#include "metron/tools/metron_tools.h"

// All the combos of tasks/funcs should work from ticks and tocks.

class Module {
public:
  Module() {
    my_reg = 0;
    counter = 0;
  }

  logic<32> my_sig;

  logic<1> done() {
    return counter > 1000;
  }

  logic<32> result() {
    return my_reg;
  }

  void tock() {
    my_sig = private_func2(171) + 7;
    tick();
  }

private:

  logic<32> private_func2(logic<8> x) {
    return my_reg + private_func(51);
  }

  void tick() {
    private_task(private_func(33));
    counter = counter + 1;
  }

  void private_task(logic<32> x) {
    my_reg = my_reg - private_func(17);
  }

  logic<32> private_func(logic<32> y) {
    return (my_reg >> 3) + y;
  }

  logic<32> my_reg;
  logic<32> counter;
};
