#include "metron_tools.h"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal
// verilator lint_off unusedparam

template <uint32_t addr_mask = 0xF0000000, uint32_t addr_tag = 0x00000000>
class test_reg {
public:

  test_reg(logic<32> init = 0) {
    data = init;
  }

  void tick(logic<1> wren, logic<32> new_data) {
    if (wren) {
      data = new_data;
    }
  }

  logic<32> data;
};

class test_top {
public:

  void tock() {
    debug_reg.tick(1, 0x12345678);
  }

  test_reg<0xF0000000> debug_reg;
};

// verilator lint_on unusedsignal
// verilator lint_off unusedparam
//------------------------------------------------------------------------------
