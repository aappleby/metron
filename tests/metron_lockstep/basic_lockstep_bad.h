#include "metron_tools.h"

// Force a mismatch between the Metron and Verilator sims so we can ensure that
// we catch them.

class Module {
public:
  Module() {
    counter = 0;
  }

  logic<1> done() {
    return counter >= 7;
  }

  logic<32> result() {
    logic<32> c = counter;
    /*#
    c = c + 1;
    #*/
    return c;
  }

  void tock() {
    tick();
  }

private:

  void tick() {
    counter = counter + 1;
  }

  logic<32> counter;

};
