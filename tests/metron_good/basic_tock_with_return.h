#include "metron_tools.h"

// Tock methods can return values.

class Module {
public:

  logic<8> tock() {
    return 123;
  }
};
