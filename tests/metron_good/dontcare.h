#include "metron_tools.h"

// DONTCARE gets translated to 1'bx

class Module {
public:

  // FIXME why is this broken?
  /*
  logic<8> test1() {
    return DONTCARE;
  }
  */

  logic<8> test2() {
    return b8(DONTCARE);
  }

  logic<8> test3() {
    return bx<8>(DONTCARE);
  }
};
