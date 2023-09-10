#include "metron/tools/metron_tools.h"

class Module {
public:

  Module(int q = 7) {
    x = 1;
    y = 2;
    z = 3;
  }

private:

  logic<1> x;
  logic<1> y;
  logic<1> z;
  logic<2> out;
};
