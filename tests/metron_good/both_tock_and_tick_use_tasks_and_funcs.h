#include "metron_tools.h"

// All the combos of tasks/funcs should work from ticks and tocks.

class Module {
public:

  logic<8> my_sig;

  void tock() {
    public_task(public_func(17));
    tick();
  }

  void public_task(logic<8> x) {
    my_sig = x + 7;
  }

  logic<8> public_func(logic<8> x) {
    return my_reg + private_func(5);
  }

private:

  void tick() {
    private_task(private_func(33));
  }

  void private_task(logic<8> x) {
    my_reg = my_reg + private_func(16);
  }

  logic<8> private_func(logic<8> y) {
    return my_reg + y;
  }

  logic<8> my_reg;
};
