#include "metron/metron_tools.h"

// Intentionally-failing test that should trigger the lockstep test timeout

class Module {
public:
  Module() {
    counter = 0;
  }

  logic<1> done() {
    return counter >= 0xFFFF0000;
  }

  logic<32> result() {
    return counter;
  }

  void tock() {
    tick();
  }

private:

  void tick() {
    counter = counter + 1;
  }

  logic<32> counter;

};
