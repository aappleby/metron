#include "metron_tools.h"

// Calling public methods from a tick() is not allowed.

//X Can't call public method set_sig from private tick

class Module {
public:

  void tock1() {
    tick();
  }

  void set_sig() {
    sig = 1;
  }

private:

  void tick() {
    reg = 0;
    set_sig();
  }

  logic<1> reg;
  logic<1> sig;
};
