#include "metron/tools/metron_tools.h"

struct Flep {
  int x;
  int y;
  int z;
};

class Module {
public:

  Flep tock(Flep x) {
    Flep newFlep;
    newFlep.y = x.y + 1;
    return newFlep;
  }

  void tick() {
    foo = foo + 1;
  }

private:
  int foo;
};
