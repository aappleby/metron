#include "metron_tools.h"

// The tracer should catch it if we add a redundant break.

class Module {
public:

  logic<8> tock() {
    logic<8> result;
    switch(my_reg) {
      case 0:
        result = 10;
        break;
        result = 10;
        break;
      default:
        result = 30;
        break;
    }
    return result;
  }

  void tick() {
    my_reg = my_reg + 1;
  }

private:

  logic<8> my_reg;
};
