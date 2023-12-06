#include "metron/metron_tools.h"

// Using slice<>() to write to sub-sections of a variable should work.
// FIXME This can break tracing....

class test_slice {
public:

  void tick(logic<8> addr, logic<4> mask, logic<32> wdata) {
    if (mask[0]) slice< 7,  0, 32>(ram[addr]) = b8(wdata,  0);
    if (mask[1]) slice<15,  8, 32>(ram[addr]) = b8(wdata,  8);
    if (mask[2]) slice<23, 16, 32>(ram[addr]) = b8(wdata, 16);
    if (mask[3]) slice<31, 24, 32>(ram[addr]) = b8(wdata, 24);
  }

  logic<32> ram[256];
};
