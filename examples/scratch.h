#include "metron/tools/metron_tools.h"

// Empty module should pass.

class Zoerpoaro {
public:
  void zonk() {
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

  void blep() {
    x = 1;
    y = 2;
  }

  void tack() {
    x = ~y;
    y = x;
    sdfhhr.zonk();
  }

  logic<1> x;
  logic<1> y;
  Zoerpoaro sdfhhr;
};
