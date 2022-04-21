#include "metron_tools.h"

// Registers that are read after they're written should fail.

//X Reading field reg changed state from FIELD_REGISTER to FIELD_INVALID

class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    logic<1> x = reg;
    reg = 1;
    logic<1> y = reg;
  }

  logic<1> reg;
};
