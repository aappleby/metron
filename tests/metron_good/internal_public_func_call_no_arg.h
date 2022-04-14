#include "metron_tools.h"

// We can call a function with no params from inside the module.

class Module {
public:

  int foo() const {
    return 2;
  }

  void tock() {
    tick();
  }

private:

  logic<3> my_reg;

  void tick() {
    my_reg = foo() + 1;
  }

};
