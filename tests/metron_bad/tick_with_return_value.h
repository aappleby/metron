#include "metron_tools.h"

// Tick methods are not allowed to have a return value.

class Module {
public:
  
  logic<2> tick() {
    my_reg = my_reg + 1;
    return 8;
  }

private:

  logic<2> my_reg;

};
