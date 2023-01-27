#include "metron_tools.h"

// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

struct InnerStruct {
  logic<8> a;
  logic<8> b;
  logic<8> c;
};

class Submodule {
public:
  logic<8> d;
  logic<8> e;
  logic<8> f;
};

class Module {
public:

  Submodule sm;

  logic<8> a;
  logic<8> b;
  logic<8> c;

  void func1(InnerStruct is, logic<8> derp) {
    sm.d = a;
    sm.e = b;
    sm.f = c;
    a = is.c + derp;
    b = is.b + derp;
    c = is.a + derp;
  }
};
