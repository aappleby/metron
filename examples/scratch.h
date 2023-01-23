#pragma once
#include "metron_tools.h"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal
// verilator lint_off unusedparam

template <uint32_t addr_mask = 0xF0000000, uint32_t addr_tag = 0x00000000>
class test_reg {
public:

  test_reg(logic<32> init) {
  }

  void tick() {
  }
};

// verilator lint_on unusedsignal
// verilator lint_off unusedparam
//------------------------------------------------------------------------------
