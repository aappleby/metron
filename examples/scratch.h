#pragma once
#include "metron_tools.h"
#include "tilelink.h"

//------------------------------------------------------------------------------

// verilator lint_off unusedsignal
// verilator lint_off undriven

class block_ram {
public:

  block_ram(const char* filename = nullptr) {
    if (filename) readmemh(filename, data);
  }

  logic<32> rdata() const {
    return data_out;
  }

  void tick(tilelink_a tla, logic<12> addr, logic<1> cs, logic<32> wdata, logic<4> wmask, logic<1> wren) {
    if (cs && wren) {
      logic<32> old_data = data[b10(tla.a_address, 2)];
      logic<32> new_data = tla.a_data;
      if (addr[0]) new_data = new_data << 8;
      if (addr[1]) new_data = new_data << 16;
      new_data = ((wmask[0] ? new_data : old_data) & 0x000000FF) |
                 ((wmask[1] ? new_data : old_data) & 0x0000FF00) |
                 ((wmask[2] ? new_data : old_data) & 0x00FF0000) |
                 ((wmask[3] ? new_data : old_data) & 0xFF000000);

      data[b10(addr, 2)] = new_data;
      data_out = cs ? new_data : b32(0);
    }
    else {
      data_out = cs ? data[b10(addr, 2)] : b32(0);
    }
  }

  // metron_noconvert
  uint32_t* get_data() { return (uint32_t*)data; }
  // metron_noconvert
  const uint32_t* get_data() const { return (uint32_t*)data; }

  // metron_internal
  logic<32> data[16384];
  logic<32> data_out;
};

// verilator lint_on unusedsignal
// verilator lint_on undriven

//------------------------------------------------------------------------------
