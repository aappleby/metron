#include "metron/metron_tools.h"

// Public register member variables get moved to the output port list.

class Module {
public:
  void tock() {
    tick();
  }
  logic<1> my_reg_;

private:
  void tick() {
    my_reg_ = my_reg_ + 1;
  }
};
