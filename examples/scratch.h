#include "metron/tools/metron_tools.h"

// Public register member variables get moved to the output port list.

class Module {
public:

  void tick() {
    if (1) {
      my_reg = my_reg + 2;
    }
    else {
      my_reg = my_reg + 1;
    }
  }

  logic<1> my_reg;
};
