#include "metron_tools.h"

// Tick inputs should become input ports.

class Module {
public:

  void tick(logic<7> my_input) {
    my_reg = my_reg + my_input;
  }

  logic<7> my_reg;
};
