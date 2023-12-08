#include "metron/metron_tools.h"

struct InnerStruct {
  logic<8> a;
  logic<8> b;
  logic<8> c;
};

struct OuterStruct {
  InnerStruct x;
  InnerStruct y;
  InnerStruct z;
};

class Module {
public:

  OuterStruct r;
  OuterStruct s_;

  void tock_func1() {
    r.x.a = 1;
    r.x.b = 2;
    r.x.c = 3;
  }

  void tick_func2() {
    s_.y.a = 4;
    s_.y.b = 5;
    s_.y.c = 6;
  }

  void tick_func3() {
    s_.z.a = 7;
    s_.z.b = 8;
    s_.z.c = 9;
  }
};
