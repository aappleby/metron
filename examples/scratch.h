#include "metron/tools/metron_tools.h"

// Empty module should pass.

class Zoerpoaro {
};

struct Vec3 {
  float x;
  float y;
  float z;
};

class Module {
  Module() {
    blep();
  }

  void blep() {
    x = 1;
    y = 2;
  }

  logic<1> x;
  logic<1> y;

  void tick() {
    x = ~y;
    y = x;
  }

  Zoerpoaro sdfhhr;
};
