#include "metron/metron_tools.h"

union my_union {
  uint32_t a;
  uint32_t b;
};

class Module {
public:

  void tock(uint32_t x, uint32_t y) {
    blah.a = x;
    blah.b = y;
  }

  my_union blah;
};
