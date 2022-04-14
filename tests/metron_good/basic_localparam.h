#include "metron_tools.h"

// Static const members become SV localparams

class Module {
public:

  static const int my_val = 7;

  void tick() {
    my_reg = my_val;
  }

  logic<7> my_reg;
};
