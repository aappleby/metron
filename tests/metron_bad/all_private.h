#include "metron_tools.h"

// Only public methods are traced. If everything's private, we should trigger
// a missed field error.

class Module {
public:

private:
  void tock() {
    sig = reg;
  }

  void tick() {
    reg = 1;
  }

  logic<1> sig;
  logic<1> reg;
};
