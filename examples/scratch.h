#include "metron/tools/metron_tools.h"

// Empty module should pass.

class Module {
  logic<1> x;
  logic<1> y;

  void tick() {
    x = ~y;
    y = x;
  }
};
