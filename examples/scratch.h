#include "metron_tools.h"

// Number literals
// don't forget the ' spacers

class Submod {
public:
  int tock(int x) {
    return x + 3;
  }
};

class Module {
public:

  int tock(int x) {
    tick(x);
    return submod.tock(x) + 3;
  }

  void tick(int x) {
    my_reg = my_reg + x;
  }

  Submod submod;

  int my_reg;
};
