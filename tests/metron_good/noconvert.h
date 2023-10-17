#include "metron/metron_tools.h"

class Module {
  public:

  logic<1> foo() {
    return 0;
  }

  // metron_noconvert
  int bar() {
    return 0xDEADBEEF;
  }

};
