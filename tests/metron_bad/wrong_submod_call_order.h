#include "metron/metron_tools.h"

// If a module calls a submod's functions in the "wrong" order, we should catch
// it.

#include "metron/metron_tools.h"

class Submod {
public:

  void tick1() { y_ = y_ + z_; }
  void tick2() { z_ = z_ + 1; }

private:
  int y_;
  int z_;
};

class Module {
public:

  void tock() {
    x.tick2();
    x.tick1();
  }

  Submod x;
};
