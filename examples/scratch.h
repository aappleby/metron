#include "metron/tools/metron_tools.h"

class Module {
public:

  logic<3> tock(logic<1> q) {
    out = x + y + z + q;
    return 7;
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
