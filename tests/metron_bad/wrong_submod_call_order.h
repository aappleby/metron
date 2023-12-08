#include "metron/metron_tools.h"

// If a module calls a submod's functions in the "wrong" order, we should catch
// it.

#include "metron/metron_tools.h"

class Submod {
public:

  void tick1() { y = y + z; }
  void tick2() { z = z + 1; }

private:
  int y;
  int z;
};

class Module {
public:

  void tock() {
    x_.tick2();
    x_.tick1();
  }

private:

  Submod x_;
};
