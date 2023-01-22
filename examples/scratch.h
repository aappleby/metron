#pragma once
#include "metron_tools.h"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal
// verilator lint_off undriven

struct tilelink_a {
  logic<32> a_data;
};

class block_ram {
public:

  logic<32> unshell(tilelink_a tla) {
    return tla.a_data;
  }

  void tick(tilelink_a tla) {
    data = tla.a_data;
  }

  logic<32> data;
};

// verilator lint_on unusedsignal
// verilator lint_on undriven

//------------------------------------------------------------------------------
