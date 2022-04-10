#include "metron_tools.h"

// Basic #defines should work

#define MY_CONSTANT1 10
#define MY_CONSTANT2 20
#define MY_OTHER_CONSTANT (MY_CONSTANT1 + MY_CONSTANT2 + 7)

class Module {
public:

  logic<8> tock() {
    return MY_OTHER_CONSTANT;
  }

};
