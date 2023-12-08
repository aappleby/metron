#include "metron/metron_tools.h"

// Static const members become SV localparams

class Module {
public:

  void tock() {
    tick();
  }

private:

  static const int my_val = 7;

  void tick() {
    my_reg_ = my_reg_ + my_val;
  }

  logic<7> my_reg_;
};
