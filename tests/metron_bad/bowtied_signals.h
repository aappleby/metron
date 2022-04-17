#include "metron_tools.h"

// Tock methods might be valid individually, but not when combined into one
// module/

//X Writing field sig2 changed state from FIELD_INPUT to FIELD_INVALID

class Module {
public:

  void tock1() {
    sig1 = sig2;
  }

  void tock2() {
    sig2 = sig1;
  }

private:
  logic<8> sig1;
  logic<8> sig2;
};
