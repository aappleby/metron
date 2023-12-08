#include "metron/metron_tools.h"

// Modules can contain other modules.

class Submod {
public:

  void tock(int x) {
    tick(x);
  }

private:

  void tick(int x) {
    sub_reg_ = sub_reg_ + x;
  }

  logic<8> sub_reg_;
};

class Module {
public:

  void tock(int x) {
    submod_.tock(x);
  }

  Submod submod_;
};
