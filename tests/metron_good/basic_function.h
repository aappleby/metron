#include "metron/metron_tools.h"

// Methods that don't write anything should become functions.

class Module {
public:

  void tick() {
    my_reg_ = my_reg_ + some_func();
  }

  logic<8> some_func() const {
    return 3;
  }

  logic<8> my_reg_;
};
