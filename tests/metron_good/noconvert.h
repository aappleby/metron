#include "metron/metron_tools.h"

class Module {
  public:

  logic<1> foo() {
    // metron_noconvert
    int x;
    return 0;
  }

  // metron_noconvert
  void debug_dump() {
    printf("Hello World %d\n", 1234);
  }
};
