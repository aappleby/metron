#include "metron_tools.h"

// Simple switch statements are OK.

class Module {
public:

  void tick(logic<2> selector) {
    switch(selector) {
      case 0: my_reg = 17; break;
      case 1: my_reg = 22; break;
      case 2: my_reg = 30; break;
      case 3: my_reg = 72; break;
    }
  }

private:
  logic<8> my_reg;
};
