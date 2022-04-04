#include "metron_tools.h"

// Declaration order _matters_ - a tick() before a tock() that reads the ticked
// reg should fail validation.

class Module {
public:

  void tick() {
    reg = 1;
  }

  void tock() {
    sig = reg;
  }

  logic<1> sig;
  logic<1> reg;
};
