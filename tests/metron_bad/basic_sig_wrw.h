#include "metron_tools.h"

// Signals that are written after they're read should fail.

// X Don't know how to categorize sig = CTX_INVALID

class Module {
 public:
  void tock() {
    sig = 1;
    logic<1> x = sig;
    sig = 0;
  }

  logic<1> sig;
};
