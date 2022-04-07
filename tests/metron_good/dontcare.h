#include "metron_tools.h"

// DONTCARE gets translated to 1'bx

class Module {
public:

  //logic<8> tock1() {
  //  return DONTCARE;
  //}

  logic<8> tock2() {
    return b8(DONTCARE);
  }

  logic<8> tock3() {
    return bx<8>(DONTCARE);
  }
};
