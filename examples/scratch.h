#include "metron_tools.h"

// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

struct InnerStruct {
  logic<8> a;
  logic<8> b;
  logic<8> c;
  logic<32> d[7];
};

struct OuterStruct {
  InnerStruct x;
  InnerStruct y;
  InnerStruct z;
};

class Mipule {
  public:

  void tock(logic<7> slkdfjlskdj) {
  }

  logic<7> mippy;
};

class Module {
public:

  Mipule the_mip;

  OuterStruct s;

  logic<32> array_field[7];

  void func1(logic<7> blerp) {
    s.x.a = 1;
    s.x.b = 2 + blerp;
    s.x.c = 3;
  }

  void func2(OuterStruct clarnk) {
    s.y.a = 4;
    s.y.b = 5;
    s.y.c = 6;
  }

  void func3(InnerStruct querz) {
    s.z.a = 7;
    s.z.b = 8;
    s.z.c = 9;
  }
};
