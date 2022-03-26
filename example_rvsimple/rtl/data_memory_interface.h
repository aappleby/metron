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
  logic<32> read_data;
  logic<32> bus_write_data;
  logic<4> bus_byte_enable;

 private:
  logic<32> position_fix;
  logic<32> sign_fix;

 public:
  void tock_bus(logic<3> data_format,
                logic<32> address,
                logic<32> write_data) {
    bus_write_data = write_data << (8 * b2(address));

    // calculate byte enable
    switch (b2(data_format)) {
      case 0b00:
        bus_byte_enable = b4(0b0001) << b2(address);
        break;
      case 0b01:
        bus_byte_enable = b4(0b0011) << b2(address);
        break;
      case 0b10:
        bus_byte_enable = b4(0b1111) << b2(address);
        break;
      default:
        bus_byte_enable = b4(0b0000);
        break;
    }
  }

  // correct for unaligned accesses
  void tock_position_fix(logic<32> address,
                         logic<32> bus_read_data) {
    position_fix = b32(bus_read_data >> (8 * b2(address)));
  }

  // sign-extend if necessary
  void tock_sign_fix(logic<3> data_format) {

    switch (b2(data_format)) {
      case 0b00:
        sign_fix = cat(dup<24>(b1(~data_format[2] & position_fix[7])),
                       b8(position_fix));
        break;
      case 0b01:
        sign_fix = cat(dup<16>(b1(~data_format[2] & position_fix[15])),
                       b16(position_fix));
        break;
      case 0b10:
        sign_fix = b32(position_fix);
        break;
      default:
        sign_fix = b32(DONTCARE);
        break;
    }
  }

  void tock_read_data() { read_data = sign_fix; }
};

#endif  // RVSIMPLE_DATA_MEMORY_INTERFACE_H
