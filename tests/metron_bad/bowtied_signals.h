#include "metron/metron_tools.h"

// X Context sig1 had invalid state after trace

class Module {
 public:
  void tock1() { sig1 = sig2; }

  void tock2() { sig2 = sig1; }

  void tock3() { sig1 = 0; }

 private:
  logic<8> sig1;
  logic<8> sig2;
};
