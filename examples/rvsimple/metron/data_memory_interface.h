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
  logic<32> address2;
  logic<3> data_format2;
  logic<32> bus_read_data;
  logic<32> address;
  logic<3> data_format;
  logic<32> write_data;
  logic<32> bus_write_data;
  logic<32> read_data;
  logic<4> bus_byte_enable;

  void tock_inputs() {
    address2 = address;
    data_format2 = data_format;
  }

  void tock_bus_write_data() {
    bus_write_data = write_data << (8 * b2(address2));
  }

  // calculate byte enable
  void tock_bus_byte_enable() {
    // clang-format off
    switch (b2(data_format2)) {
      case 0b00: bus_byte_enable = b4(0b0001) << b2(address2); break;
      case 0b01: bus_byte_enable = b4(0b0011) << b2(address2); break;
      case 0b10: bus_byte_enable = b4(0b1111) << b2(address2); break;
      default:   bus_byte_enable = b4(0b0000); break;
    }
    // clang-format on
  }

  // correct for unaligned accesses
  void tock_read_data() {
    logic<32> position_fix = b32(bus_read_data >> (8 * b2(address2)));

    // sign-extend if necessary
    // clang-format off
    switch (b2(data_format2)) {
      case 0b00: read_data = cat(dup<24>(b1(~data_format2[2] & position_fix[7])), b8(position_fix)); break;
      case 0b01: read_data = cat(dup<16>(b1(~data_format2[2] & position_fix[15])), b16(position_fix)); break;
      case 0b10: read_data = b32(position_fix); break;
      default:   read_data = b32(DONTCARE); break;
    }
    // clang-format on
  }
};

#endif  // RVSIMPLE_DATA_MEMORY_INTERFACE_H
