#include "metron_tools.h"

// Concatenating logics should produce logics with correct <N>

class Module {
public:

  void init() {
    write("Hello World?\n");
  }

  void tock1() {
    logic<1> a = 1;
    logic<2> b = 2;
    logic<3> c = 3;

    logic<6> d = cat(a, b, c);
  }

  void tock2() {
    logic<8> a = 1;
    logic<8> b = 2;
    logic<8> c = 3;

    logic<6> d = cat(b1(a), b2(b), b3(c));
  }
};
