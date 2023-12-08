#include "metron/metron_tools.h"

// Private const methods should turn into SV functions.

class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    my_reg_ = my_reg_ + some_func();
  }

  logic<8> some_func() const {
    return 3;
  }

  logic<8> my_reg_;
};
