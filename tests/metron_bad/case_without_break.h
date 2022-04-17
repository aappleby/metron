#include "metron_tools.h"

// The tracer should catch it if we forget a break.
//X  Case statement doesn't end with break.

class Module {
public:

  logic<8> tock() {
    logic<8> result;
    switch(my_reg) {
      case 0: // can we stick comments in here?
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

    tick();
    return result;
  }

private:

  void tick() {
    my_reg = my_reg + 1;
  }

  logic<8> my_reg;
};
