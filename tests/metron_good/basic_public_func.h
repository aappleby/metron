#include "metron_tools.h"

// Public functions should turn into always_comb and their args should turn
// into input ports.

class Module {
public:

  logic<8> func1() const {
    return 23;
  }

  logic<8> func2(logic<8> arg) const {
    return 17 + arg;
  }

  void tock() {
    tick();
  }

private:

  void tick() {
    my_reg = 1;
  }

  logic<1> my_reg;
};
