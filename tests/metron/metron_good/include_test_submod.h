// Metron should be able to handle #included submodules.

#include "metron/metron_tools.h"

class Submod {
public:

  void tick(int x) {
    y_ += x;
  }

private:
  int y_;
};
