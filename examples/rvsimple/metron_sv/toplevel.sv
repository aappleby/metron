// RISC-V SiMPLE SV -- Toplevel
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_TOPLEVEL_H
`define RVSIMPLE_TOPLEVEL_H

`include "config.sv"
`include "constants.sv"
`include "example_data_memory_bus.sv"
`include "example_text_memory_bus.sv"
`include "metron_tools.sv"
`include "riscv_core.sv"

module toplevel
(
  input logic clock,
  input logic reset,
  output logic[31:0] o_bus_read_data,
  output logic[31:0] o_bus_address,
  output logic[31:0] o_bus_write_data,
  output logic[3:0]  o_bus_byte_enable,
  output logic  o_bus_read_enable,
  output logic  o_bus_write_enable,
  output logic[31:0] o_inst,
  output logic[31:0] o_pc
);
 /*public:*/
  /*logic<32> o_bus_read_data;*/
  /*logic<32> o_bus_address;*/
  /*logic<32> o_bus_write_data;*/
  /*logic<4>  o_bus_byte_enable;*/
  /*logic<1>  o_bus_read_enable;*/
  /*logic<1>  o_bus_write_enable;*/
  /*logic<32> o_inst;*/
  /*logic<32> o_pc;*/

  //----------------------------------------

  always_comb begin /*tock*/
    /*core.tock_pc()*/;

    text_memory_bus_address = core_pc;
    /*text_memory_bus.tock_read_data()*/;

    core_inst = text_memory_bus_read_data;
    /*core.tock_inst()*/;
    /*core.tock_alu_result()*/;
    /*core.tock_bus_write_data2()*/;
    /*core.tock_bus_write_enable2()*/;
    /*core.tock_bus_byte_enable2()*/;
    /*core.tock_bus_read_enable2()*/;

    data_memory_bus_address = core_alu_result;
    data_memory_bus_read_enable = core_bus_read_enable2;
    /*data_memory_bus.tock_q()*/;

    o_inst = text_memory_bus_read_data;
    o_bus_read_data = data_memory_bus_q;
    o_bus_address = core_alu_result;
    o_bus_write_data = core_bus_write_data;
    o_bus_byte_enable = core_bus_byte_enable2;
    o_bus_read_enable = core_bus_read_enable2;
    o_bus_write_enable = core_bus_write_enable2;
    o_pc = core_pc;

    data_memory_bus_write_enable = core_bus_write_enable2;
    data_memory_bus_byte_enable = core_bus_byte_enable2;
    data_memory_bus_write_data = core_bus_write_data;
    /*data_memory_bus.tock()*/;

    core_reset = reset;
    core_bus_read_data = data_memory_bus_q;
    /*core.tock()*/;
  end

  //----------------------------------------

 /*private:*/
  riscv_core core(
    // Inputs
    .clock(clock),
    .inst(core_inst), 
    .reset(core_reset), 
    .bus_read_data(core_bus_read_data), 
    // Outputs
    .alu_result(core_alu_result), 
    .bus_write_data(core_bus_write_data), 
    .pc(core_pc), 
    .bus_write_enable2(core_bus_write_enable2), 
    .bus_byte_enable2(core_bus_byte_enable2), 
    .bus_read_enable2(core_bus_read_enable2)
  );
  logic[31:0] core_inst;
  logic core_reset;
  logic[31:0] core_bus_read_data;
  logic[31:0] core_alu_result;
  logic[31:0] core_bus_write_data;
  logic[31:0] core_pc;
  logic core_bus_write_enable2;
  logic[3:0] core_bus_byte_enable2;
  logic core_bus_read_enable2;

  example_text_memory_bus text_memory_bus(
    // Inputs
    .clock(clock),
    .address(text_memory_bus_address), 
    // Outputs
    .read_data(text_memory_bus_read_data)
  );
  logic[31:0] text_memory_bus_address;
  logic[31:0] text_memory_bus_read_data;

  example_data_memory_bus data_memory_bus(
    // Inputs
    .clock(clock),
    .address(data_memory_bus_address), 
    .read_enable(data_memory_bus_read_enable), 
    .write_enable(data_memory_bus_write_enable), 
    .byte_enable(data_memory_bus_byte_enable), 
    .write_data(data_memory_bus_write_data), 
    // Outputs
    .q(data_memory_bus_q)
  );
  logic[31:0] data_memory_bus_address;
  logic data_memory_bus_read_enable;
  logic data_memory_bus_write_enable;
  logic[3:0] data_memory_bus_byte_enable;
  logic[31:0] data_memory_bus_write_data;
  logic[31:0] data_memory_bus_q;

endmodule;

`endif  // RVSIMPLE_TOPLEVEL_H

