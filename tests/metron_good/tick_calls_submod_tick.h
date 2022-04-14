#include "metron_tools.h"

class Submod {
public:

  void tick() {
    my_reg = my_reg + 1;
  }

private:
  logic<8> my_reg;
};

class Module {
public:

  void tick() {
    submod.tick();
  }

private:
  Submod submod;
};
