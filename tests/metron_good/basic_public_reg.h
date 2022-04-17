#include "metron_tools.h"

// Public register member variables get moved to the output port list.

class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    my_reg = 1;
  }

  logic<1> my_reg;
};
