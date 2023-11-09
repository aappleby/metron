#include "metron/metron_tools.h"

union my_union {
  logic<32> a;
  logic<32> b;
};

class Module {
public:

  void tock(logic<32> x, logic<32> y) {
    blah.a = x;
    blah.b = y;
  }

  my_union blah;
};
