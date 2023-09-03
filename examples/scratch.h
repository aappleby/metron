#include "metron/tools/metron_tools.h"

class Submod {
  void tack() {
    x = y + 1;
  }

  int x;
  int y;
};

class Module {
public:

  void tick() {
    /*
    if (1) {
      z = 2;
    }
    else {
      z = 3;
    }
    */
    submod.tack();
  }

  Submod submod;
  int z;
};
