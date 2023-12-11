#include "metron/metron_tools.h"

// Force a mismatch between the Metron and Verilator sims so we can ensure that
// we catch them.

class Module {
public:
  Module() {
    counter_ = 0;
  }

  logic<1> done() {
    return counter_ >= 7;
  }

  logic<32> result() {
    logic<32> c = counter_;
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
    counter_ = counter_ + 1;
  }

  logic<32> counter_;

};
