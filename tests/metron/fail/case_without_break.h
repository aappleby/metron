#include "metron/metron_tools.h"

// The tracer should catch it if we forget a break.
// X Case statement in tock does not end with break

class Module {
 public:
  logic<8> tock(logic<8> in) {
    logic<8> result;
    switch (in) {
      case 0:
      case 1:
      case 2:
        result = 10;
        break;
      case 3: {
        result = 20;
        /*break;*/
      }
      default:
        result = 30;
        break;
    }

    return result;
  }
};
