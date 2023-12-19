// Metron should be able to handle #included submodules.

#include "include_test_submod.h"

class Module {
public:

  void tick(int x) {
    submod.tick(x);
  }

private:
  Submod submod;
};
