#include "metron_tools.h"

// Trying to use the return value from increment/decrement should be caught by
// the compiler.

class Module {
public:
  logic<8> tock() {
    logic<8> temp = 3;
    return temp++;
  }
};
