#include "metron/metron_tools.h"

// All the combos of tasks/funcs should work from ticks and tocks.

class Module {
public:
  Module() {
    my_reg_ = 0;
    counter_ = 0;
  }

  logic<32> my_sig;

  logic<1> done() {
    return counter_ > 1000;
  }

  logic<32> result() {
    return my_reg_;
  }

  void tock() {
    my_sig = private_func2(171) + 7;
    tick();
  }

private:

  logic<32> private_func2(logic<8> x) {
    return my_reg_ + private_func(51);
  }

  void tick() {
    private_task(private_func(33));
    counter_ = counter_ + 1;
  }

  void private_task(logic<32> x) {
    my_reg_ = my_reg_ - private_func(17);
  }

  logic<32> private_func(logic<32> y) {
    return (my_reg_ >> 3) + y;
  }

  logic<32> my_reg_;
  logic<32> counter_;
};
