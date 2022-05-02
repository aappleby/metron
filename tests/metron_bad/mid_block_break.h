#include "metron_tools.h"

// The tracer should catch it if we add a redundant break and it's in a block.

// X Case statement in tock does not end with break

class Module {
 public:
  logic<8> tock() {
    logic<8> result;
    switch (my_reg) {
      case 0: {
        result = 10;
        break;
        result = 10;
        break;
      }
      default:
        result = 30;
        break;
    }

    tick();
    return result;
  }

 private:
  void tick() { my_reg = my_reg + 1; }

  logic<8> my_reg;
};
