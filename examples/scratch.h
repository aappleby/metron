#include "metron/tools/metron_tools.h"

/*
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
*/

class Module {
public:

  int x;
  int y;

  void tock(int d) {
    y = x + d;
    x = y + d;
  }

  void tick() {
    tock(2);
  }

  /*
  Foo sjdlfk;
  int jwioerj;
  logic<8> blarp;

  int blarg(int x) {
    return 2;
  }

  int tock(int a, int b, int c) {
    return a * b + c;
    return 1;
  }
  */
};
