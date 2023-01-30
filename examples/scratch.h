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

  InnerStruct func2(OuterStruct clarnk) {
    clarnk.x.a = clarnk.x.a + 1;

    clarnk.y.a = 7;
    logic<8> blep = clarnk.y.a;

    s.y.a = clarnk.y.d[2];
    s.y.d[6] = 5;
    s.y.a = 6;
    return s.x;
  }
};
