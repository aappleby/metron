#include <stdio.h>
#include "metron_tools.h"

class lfsr24 {
public:
  logic<24> state;

  void update(logic<1> reset, logic<1> enable) {
    if (reset) {
      state = 1;
    }
    else if (enable) {
      logic<1> next_bit = state[23] ^ state[22] ^ state[21] ^ state[16];
      state = (state << 1) | next_bit;
    }
  }
};


int main(int argc, char** argv) {
  printf("Running lfsr24 benchmark\n");

  lfsr24 dut;
  dut.update(1, 0);
  for (int i = 0; i < (16 * 1024 * 1024) - 1; i++) {
    dut.update(0, 1);
  }
  printf("Final lfsr state was 0x%08x\n", (int)dut.state);
  return 0;
}
