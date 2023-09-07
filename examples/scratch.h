#include "metron/tools/metron_tools.h"

// Empty module should pass.

class Zoerpoaro {
public:

  static const int some_param = 7;

  int vv;

  void zonk(int q = 7) {
    vv = q + 4;
  }
};

struct Vec3 {
  float x;
  float y;
  float z;
};

class Module {
public:
  Module() {
    blep();
  }

  void tick() {
    tack();
  }

private:

  static int bleh;

  void blep() {
    x = 1;
    y = 2;
  }

  void tack() {
    x = ~y;
    y = x;
    sdfhhr.zonk(7);
  }

  logic<1> x;
  logic<1> y;
  Zoerpoaro sdfhhr;
};
