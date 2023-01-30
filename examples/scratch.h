#include "metron_tools.h"

// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

struct InnerStruct {
  logic<8> a;
  logic<32> d[7];
};

struct OuterStruct {
  InnerStruct x;
  InnerStruct y;
};

class Module {
public:
  OuterStruct s;

  void func2(OuterStruct clarnk) {
    s.y.a = clarnk.y.d[2];
    s.y.d[6] = 5;
    s.y.a = 6;
  }
};
