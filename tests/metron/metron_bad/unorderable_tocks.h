#include "metron/metron_tools.h"

// These tocks are invalid 'cause there's no valid order.

class Module {
public:

  void update_a() {
    a = b + 1;
  }

  void update_b() {
    b = a + 1;
  }

  logic<32> a;
  logic<32> b;
};
