#include "metron/tools/metron_tools.h"

class Submod1 {
public:

  int x;
  int y;
  int z;
};

class Module {
public:

  void tock_foo1() {
    tock_foo2();
    tock_foo2();
    tock_foo2();
  }

private:

  void tock_foo2() {
    tock_foo3();
  }

  void tock_foo3() {
    tock_foo4();
  }

  void tock_foo4() {
  }


  Submod1 x;
  Submod1 y;
  Submod1 z;
};
