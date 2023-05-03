#include "metron/tools/metron_tools.h"

class Module {
public:
  Module() {
    counter = 0;
  }

  logic<1> done() {
    return counter >= 100;
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
