#include "metron_tools.h"

// X Don't know how to categorize sig1 = CTX_INVALID

class Module {
 public:
  void tock1() { sig1 = sig2; }

  void tock2() { sig2 = sig1; }

  void tock3() { sig1 = 0; }

 private:
  logic<8> sig1;
  logic<8> sig2;
};
