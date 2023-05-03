#include "metron/tools/metron_tools.h"

// Signals that are written after they're read should fail.

// X Context sig had invalid state after trace

class Module {
 public:
  void tock() {
    sig = 1;
    logic<1> x = sig;
    sig = 0;
  }

  logic<1> sig;
};
