#include "metron_tools.h"

// Fields can be written in tick() or tock() but not both.
//X Writing field field changed state from FIELD_OUTPUT to FIELD_INVALID

class Module {
public:

  void tock() {
    field = 0;
    tick();
  }

private:

  void tick() {
    field = 1;
  }

  logic<1> field;
};
