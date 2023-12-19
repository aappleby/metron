#include "metron/metron_tools.h"

// Templates can be used for module parameters

template<int WIDTH = 123, int HEIGHT = 456>
class Submod {
public:

  void tock(logic<WIDTH> dx, logic<HEIGHT> dy) {
    my_width = bx<WIDTH>(100) + dx;
    my_height = bx<HEIGHT>(200) + dy;
  }

  logic<WIDTH> my_width;
  logic<HEIGHT> my_height;
};

class Module {
public:

  logic<20> tock_ret;
  void tock() {
    submod.tock(1, 2);
    tock_ret = submod.my_width + submod.my_height;
  }

  Submod<10,11> submod;
};
