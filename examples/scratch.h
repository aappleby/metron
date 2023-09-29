#include "metron/metron_tools.h"

class Submod {
public:

  void tick() {
    y = y + 1;
  }

private:

  int y;
};

class Module {
public:

  void tock() {
    x = 1;
    s.tick();
  }

private:
  int x;
  Submod s;
};
