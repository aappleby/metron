#include "metron_tools.h"

// Calling a tock() from a tick() is not allowed.

class Module {
public:

  void tick() {
    reg = 0;
    tock();
  }

private:

  void tock() {
    sig = 1;
  }

  logic<1> reg;
  logic<1> sig;
};
