// RISC-V SiMPLE SV -- data memory interface
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_DATA_MEMORY_INTERFACE_H
#define RVSIMPLE_DATA_MEMORY_INTERFACE_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class data_memory_interface {
 public:

  logic<32> bus_write_data(logic<32> address, logic<32> write_data) const {
    return write_data << (8 * b2(address));
  }

  logic<4> bus_byte_enable(logic<3> data_format, logic<32> address) const {
    // calculate byte enable
    logic<4> result;
    switch (b2(data_format)) {
      case 0b00: result = b4(0b0001) << b2(address); break;
      case 0b01: result = b4(0b0011) << b2(address); break;
      case 0b10: result = b4(0b1111) << b2(address); break;
      default:   result = b4(0b0000); break;
    }
    return result;
  }

  logic<32> read_data(logic<32> address, logic<32> bus_read_data, logic<3> data_format) const {
    // correct for unaligned accesses
    logic<32> position_fix = b32(bus_read_data >> (8 * b2(address)));

    // sign-extend if necessary
    logic<32> result;
    switch (b2(data_format)) {
      case 0b00: result = cat(dup<24>(b1(~data_format[2] & position_fix[7])), b8(position_fix)); break;
      case 0b01: result = cat(dup<16>(b1(~data_format[2] & position_fix[15])), b16(position_fix)); break;
      case 0b10: result = b32(position_fix); break;
      default:   result = b32(DONTCARE); break;
    }
    return result;
  }
};

#endif  // RVSIMPLE_DATA_MEMORY_INTERFACE_H
