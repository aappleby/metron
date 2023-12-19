#include "metron/metron_tools.h"

// Template parameters become SV module parameters.

template<int SOME_CONSTANT = 7>
class Module {
public:

  void tock() {
    tick();
  }

private:

  void tick() {
    my_reg_ = my_reg_ + SOME_CONSTANT;
  }

  logic<7> my_reg_;
};
