#include "metron/metron_tools.h"

class Module {
public:

  void tock() {
    my_sig = my_reg_;
    tick();
  }

private:

  void tick() {
    my_reg_ = 1;
  }

  logic<1> my_sig;
  logic<1> my_reg_;
};
