#include "metron_tools.h"

// Calling a tock() from a tick() is not allowed.

//X Can't call tock2 from tick method tick

class Module {
public:

  void tock1() {
    tick();
  }

  void tock2() {
    sig = 1;
  }

private:

  void tick() {
    reg = 0;
    tock2();
  }

  logic<1> reg;
  logic<1> sig;
};
