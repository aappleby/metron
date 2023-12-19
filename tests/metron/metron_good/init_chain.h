#include "metron/metron_tools.h"

// I don't know why you would want to do this, but it should work.

class Module {
public:
  Module() {
    init1();
  }

  void tock() {
    tick();
  }

private:

  void tick() {
    reg1_ = reg1_ + 1;
    reg2_ = reg2_ + 1;
    reg3_ = reg3_ + 1;
    reg4_ = reg4_ + 1;
    reg5_ = reg5_ + 1;
  }

  logic<8> reg1_;
  logic<8> reg2_;
  logic<8> reg3_;
  logic<8> reg4_;
  logic<8> reg5_;

  void init1() {
    reg1_ = 1;
    init2();
  }

  void init2() {
    reg2_ = 2;
    init3();
  }

  void init3() {
    reg3_ = 3;
    init4();
  }

  void init4() {
    reg4_ = 4;
    init5();
  }

  void init5() {
    reg5_ = 5;
  }

};
