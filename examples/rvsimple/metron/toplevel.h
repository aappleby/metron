// RISC-V SiMPLE SV -- Toplevel
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_TOPLEVEL_H
#define RVSIMPLE_TOPLEVEL_H

#include "config.h"
#include "constants.h"
#include "example_data_memory_bus.h"
#include "example_text_memory_bus.h"
#include "metron_tools.h"
#include "riscv_core.h"

class toplevel {
 public:
  logic<32> o_bus_read_data;
  logic<32> o_bus_address;
  logic<32> o_bus_write_data;
  logic<4>  o_bus_byte_enable;
  logic<1>  o_bus_read_enable;
  logic<1>  o_bus_write_enable;
  logic<32> o_inst;
  logic<32> o_pc;

  //----------------------------------------

  void tock(logic<1> reset) {
    core.reset = reset;
    core.tock_datapath_pc();

    text_memory_bus.address = core.pc;
    text_memory_bus.tock_read_data();

    core.inst = text_memory_bus.read_data;
    core.tock_execute();

    data_memory_bus.address      = core.bus_address;
    data_memory_bus.read_enable  = core.bus_read_enable;
    data_memory_bus.write_enable = core.bus_write_enable;
    data_memory_bus.byte_enable  = core.bus_byte_enable;
    data_memory_bus.write_data   = core.bus_write_data;
    data_memory_bus.tock();

    core.bus_read_data = data_memory_bus.q;
    core.tock_writeback();

    //----------

    o_pc   = core.pc;
    o_inst = text_memory_bus.read_data;

    o_bus_read_data    = data_memory_bus.q;
    o_bus_address      = core.bus_address;
    o_bus_write_data   = core.bus_write_data;
    o_bus_byte_enable  = core.bus_byte_enable;
    o_bus_read_enable  = core.bus_read_enable;
    o_bus_write_enable = core.bus_write_enable;
  }

  //----------------------------------------

 private:
  riscv_core core;
  example_text_memory_bus text_memory_bus;
  example_data_memory_bus data_memory_bus;
};

#endif  // RVSIMPLE_TOPLEVEL_H


#if 0


// RISC-V SiMPLE SV -- Toplevel
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`include "config.sv"
`include "constants.sv"

module toplevel (
  input  clock,
  input  reset,

  output [31:0] bus_read_data,
  output [31:0] bus_address,
  output [31:0] bus_write_data,
  output [3:0]  bus_byte_enable,
  output        bus_read_enable,
  output        bus_write_enable,

  output [31:0] inst,
  output [31:0] pc
);

riscv_core riscv_core (
  .clock                  (clock),
  .reset                  (reset),
  .inst                   (inst),
  .pc                     (pc),
  .bus_address            (bus_address),
  .bus_read_data          (bus_read_data),
  .bus_write_data         (bus_write_data),
  .bus_read_enable        (bus_read_enable),
  .bus_write_enable       (bus_write_enable),
  .bus_byte_enable        (bus_byte_enable)
);

example_text_memory_bus text_memory_bus (
  .clock                  (clock),
  .address                (pc),
  .read_data              (inst)
);

example_data_memory_bus data_memory_bus (
  .clock                  (clock),
  .address                (bus_address),
  .read_data              (bus_read_data),
  .write_data             (bus_write_data),
  .read_enable            (bus_read_enable),
  .write_enable           (bus_write_enable),
  .byte_enable            (bus_byte_enable)
);

endmodule

#endif