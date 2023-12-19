#include "metron/metron_tools.h"

// The tracer should catch it if we add a redundant break and it's in a block.

// X Case statement in tock does not end with break

class Module {
 public:
  logic<8> tock(logic<8> in) {
    logic<8> result;
    switch (in) {
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

    return result;
  }
};
