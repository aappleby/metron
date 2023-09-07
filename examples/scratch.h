#include "metron/tools/metron_tools.h"

class Module {
public:

  void tock() {
    out = x + y + z;
  }

  void tick() {
    x = x + 1;
    y = y + 2;
    z = z + 1;
  }

private:

  logic<1> x;
  logic<1> y;
  logic<1> z;
  logic<2> out;
};
