#include "metron_tools.h"

class Module {
public:
  Module() {
    counter = 0;
    lfsr = 0xDEADBEEF;
  }

  logic<1> done() {
    return counter >= 100;
  }

  logic<32> result() {
    return lfsr;
  }

  void tock() {
    tick();
  }

private:

  void tick() {
    counter = counter + 1;
    logic<1> next_bit = lfsr[31] ^ lfsr[21] ^ lfsr[1] ^ lfsr[0];
    lfsr = cat(lfsr, next_bit);
  }

  logic<32> counter;
  logic<32> lfsr;

};
