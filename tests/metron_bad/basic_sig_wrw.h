#include "metron_tools.h"

// Signals that are written after they're read should fail.

//X Writing field sig changed state from FIELD_SIGNAL to FIELD_INVALID

class Module {
public:

  void tock() {
    sig = 1;
    logic<1> x = sig;
    sig = 0;
  }

  logic<1> sig;
};
