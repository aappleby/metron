#include "metron_tools.h"

class Submod {
public:
  int i_signal;

  void tock() {
    int x = i_signal;
  }
};

class Module {
public:
  void tock() {
    submod.i_signal = 12;
  }
  Submod submod;
};
