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
    logic[31:0] pc;
    logic[31:0] inst;
    logic[31:0] alu_result2;
    logic[31:0] write_data;
    logic  write_enable;
    logic[3:0]  byte_enable;
    logic  read_enable;
    logic[31:0] read_data;
    pc           = core_pc;
    text_memory_bus_address = pc;
    inst         = text_memory_bus_read_data;
    core_inst = inst;
    alu_result2  = core_alu_result;
    core_inst = inst;
    core_alu_result2 = alu_result2;
    write_data   = core_bus_write_data2;
    core_inst = inst;
    write_enable = core_bus_write_enable2;
    core_inst = inst;
    core_alu_result2 = alu_result2;
    byte_enable  = core_bus_byte_enable2;
    core_inst = inst;
    read_enable  = core_bus_read_enable2;
    data_memory_bus_address = alu_result2;
    data_memory_bus_read_enable = read_enable;
    read_data    = data_memory_bus_read_data;

    o_inst = inst;
    o_bus_read_data = read_data;
    o_bus_address = alu_result2;
    o_bus_write_data = write_data;
    o_bus_byte_enable = byte_enable;
    o_bus_read_enable = read_enable;
    o_bus_write_enable = write_enable;
    o_pc = pc;

    data_memory_bus_address = alu_result2;
    data_memory_bus_write_enable = write_enable;
    data_memory_bus_byte_enable = byte_enable;
    data_memory_bus_write_data = write_data;
    /*data_memory_bus.tock(alu_result2, write_enable, byte_enable, write_data);*/
    core_reset = reset;
    core_inst = inst;
    core_bus_read_data = read_data;
    core_alu_result2 = alu_result2;
    /*core.tock(reset, inst, read_data, alu_result2);*/
  end

  //----------------------------------------

 /*private:*/
  riscv_core core(
    // Inputs
    .clock(clock),
    .inst(core_inst), 
    .alu_result2(core_alu_result2), 
    .reset(core_reset), 
    .bus_read_data(core_bus_read_data), 
    // Outputs
    .bus_write_data2(core_bus_write_data2), 
    .bus_byte_enable2(core_bus_byte_enable2), 
    .bus_read_enable2(core_bus_read_enable2), 
    .bus_write_enable2(core_bus_write_enable2), 
    .pc(core_pc), 
    .alu_result(core_alu_result)
  );
  logic[31:0] core_inst;
  logic[31:0] core_alu_result2;
  logic core_reset;
  logic[31:0] core_bus_read_data;
  logic[31:0] core_bus_write_data2;
  logic[3:0]  core_bus_byte_enable2;
  logic  core_bus_read_enable2;
  logic  core_bus_write_enable2;
  logic[31:0] core_pc;
  logic[31:0] core_alu_result;

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
    .read_data(data_memory_bus_read_data)
  );
  logic[31:0] data_memory_bus_address;
  logic data_memory_bus_read_enable;
  logic data_memory_bus_write_enable;
  logic[3:0] data_memory_bus_byte_enable;
  logic[31:0] data_memory_bus_write_data;
  logic[31:0] data_memory_bus_read_data;

endmodule;

`endif  // RVSIMPLE_TOPLEVEL_H

