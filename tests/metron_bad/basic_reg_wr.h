#include "metron_tools.h"

// Registers that are read after they're written should fail.

class Module {
public:

  void tick() {
    sig = 1;
    logic<1> x = sig;
  }

  logic<1> sig;
};
