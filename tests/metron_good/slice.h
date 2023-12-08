#include "metron/metron_tools.h"

// Using slice<>() to write to sub-sections of a variable should work.
// FIXME This can break tracing....

class test_slice {
public:

  void tick(logic<8> addr, logic<4> mask, logic<32> wdata) {
    if (mask[0]) slice<8,  0>(ram_[addr]) = b8(wdata,  0);
    if (mask[1]) slice<8,  8>(ram_[addr]) = b8(wdata,  8);
    if (mask[2]) slice<8, 16>(ram_[addr]) = b8(wdata, 16);
    if (mask[3]) slice<8, 24>(ram_[addr]) = b8(wdata, 24);
  }

private:
  logic<32> ram_[256];
};
