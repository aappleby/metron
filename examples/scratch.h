#include "metron_tools.h"

// Zero-initializing structs should work for convenience.

struct MyStruct1 {
  logic<8> a;
};

class Module {
public:

  logic<8> tock() {
    MyStruct1 my_struct1 = {0};
    my_struct1.a = 1;
    return 17;
  }
};
