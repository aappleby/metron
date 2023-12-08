#include "metron/metron_tools.h"

// Cramming various statements into one line should not break anything.

class Module {
public:

  logic<8> test() { logic<8> a = 1; a = a + 7; return a; }

  void tick() { if (my_reg_ & 1) my_reg_ = my_reg_ - 7; }

  logic<8> my_reg_;

};
