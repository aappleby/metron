#include "metron/tools/metron_tools.h"

class Submod1 {
public:

  int x;
  int get_y() { return y; }
  int get_z() { return z; }

private:
  int y;
  int z;
};

class Module {
public:

  void tock_foo1() {
  }

private:

  Submod1 x;
  Submod1 y;
  Submod1 z;
};
