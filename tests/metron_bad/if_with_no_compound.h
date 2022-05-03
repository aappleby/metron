#include "metron_tools.h"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

// X If statements that contain component calls must use {}

class Submod {
 public:
  void tock() {}
};

class Module {
 public:
  void tock() {
    if (1)
      submod.tock();
    else
      submod.tock();
  }

  Submod submod;
};
