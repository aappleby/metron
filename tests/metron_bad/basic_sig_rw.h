#include "metron_tools.h"

// Signals that are read before they are written should fail.

//X Writing field sig changed state from FIELD_INPUT to FIELD_INVALID

class Module {
public:

  void tock() {
    logic<1> x = sig;
    sig = 1;
  }

  logic<1> sig;
};
