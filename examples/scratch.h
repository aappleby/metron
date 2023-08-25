#include "metron/tools/metron_tools.h"

struct Baz {
  int a;
  int b;
  int c;
};

struct Bar {
  Baz x;
  Baz y;
  Baz z;
};

struct Foo {
  Bar a;
  Bar b;
  Bar c;
};

class Module {
public:

  Foo sjdlfk;
  int jwioerj;
  logic<8> blarp;

  int blarg(int x) {
    return 2;
  }

  int tock(int a, int b, int c) {
    return ++a * b-- + ++blarg(c)--;
    return 1;
  }
};
