#include "metron_tools.h"

// Signals that are read before they are written should fail.

class Module {
public:

  void tock() {
    logic<1> x = sig;
    sig = 1;
  }

  logic<1> sig;
};
