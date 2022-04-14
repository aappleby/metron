#include "metron_tools.h"

// Fields can be written in tick() or tock() but not both.

class Module {
public:

  void tick() {
    field = 1;
  }

  void tock() {
    field = 0;
  }

  logic<1> field;
};
