#include "metron_tools.h"

// Templates can be used for module parameters

template<int WIDTH, int HEIGHT = 2>
class Submod {
public:

  void tock() {
    my_width = bx<WIDTH>(100);
    my_height = bx<HEIGHT>(200);
  }

  logic<WIDTH> my_width;
  logic<HEIGHT> my_height;
};

class Module {
public:

  void tock() {
    submodule.tock();
    logic<20> foo = submodule.my_width + submodule.my_height;
  }

  Submod<10,11> submodule;
};
