#include "metron/tools/metron_tools.h"

// If a module calls a submod's functions in the "wrong" order, we should catch
// it.

#include "metron/tools/metron_tools.h"

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
    x.tick2();
    x.tick1();
  }

private:

  Submod x;
};
