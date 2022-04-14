#include "metron_tools.h"

// We can't call a function with a param from inside the module.

class Module {
public:

  int foo(logic<8> baz) const {
    return baz + 2;
  }

  void tock() {
    tick();
  }

private:

  logic<3> my_reg;

  void tick() {
    my_reg = foo(7) + 1;
  }

};
