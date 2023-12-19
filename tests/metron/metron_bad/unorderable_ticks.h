#include "metron/metron_tools.h"

// These ticks are invalid 'cause there's no valid order.

class Module {
public:

  void update_a() {
    a_ = b_ + 1;
  }

  void update_b() {
    b_ = a_ + 1;
  }

  logic<32> a_;
  logic<32> b_;
};
