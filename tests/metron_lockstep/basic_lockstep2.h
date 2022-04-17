#include "metron_tools.h"

class Module {
public:
  Module() {
    counter = 0;
  }

  logic<1> done() {
    return counter >= 7;
  }

  logic<32> result() {
    logic<32> c = counter;
    return c;
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
