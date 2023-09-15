#include "metron/tools/metron_tools.h"

// Zero-initializing structs should work for convenience.

class Module {
public:

  int my_struct1;
  void tock() {
    my_struct1 = func(2);
  }

private:

  int func(int x) {
    return x + 1;
  }

};
