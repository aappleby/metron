#include "metron_tools.h"

// Make sure our built-in functions translate.

class Module {

  void tock() {

    logic<8> src = 100;

    logic<8> a = signed(src);
    logic<8> b = unsigned(src);
    logic<8> e = sign_extend<8>(b2(src));
    logic<8> c = clog2(100);
    logic<8> d = pow2(4);

  }

};
