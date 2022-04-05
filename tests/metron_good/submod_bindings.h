#include "metron_tools.h"

// Submodule bindings should be placed immediately before the statement
// containing the call

class Submod {
public:

  logic<8> sum_a(logic<8> a1, logic<8> a2) const { return a1 + a2; }
  logic<8> sum_b(logic<8> b1, logic<8> b2) const { return b1 + b2; }
  logic<8> sum_c(logic<8> c1, logic<8> c2) const { return c1 + c2; }
  logic<8> sum_d(logic<8> d1, logic<8> d2) const { return d1 + d2; }
};

class Module {
public:

  logic<8> test_bindings() const {
    logic<8> result;
    // Submod bindings should _not_ end up here.

    {
      // Only sum_a's bindings should be here.
      if (submod.sum_a(1, 2)) {
        // Only sum_b's bindings should be here.
        result = submod.sum_b(3,4);
      }
      else {
        // Only sum_b's bindings should be here.
        result = submod.sum_b(5,6);
      }

      // All of c/d's bindings should be here.
      if (submod.sum_c(7, 8))
        result = submod.sum_d(9,10);
      else
        result = submod.sum_d(11,12);
    }

    return result;
  }

  Submod submod;
};
