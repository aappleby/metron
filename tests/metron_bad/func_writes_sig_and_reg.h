#include "metron/metron_tools.h"

// A method that writes both a signal and a register should fail

class Module {
 public:

  void update(logic<1> in, int lksjf) {
    sig = reg + in;
    reg = sig + in;
  }

  logic<1> sig;
  logic<1> reg;
};
