#include "metron_tools.h"

// Private const methods should turn into SV functions.

class Module {
public:

  void tick() {
    my_reg = my_reg + some_func();
  }

private:

  logic<8> some_func() {
    return 3;
  }

  logic<8> my_reg;
};
