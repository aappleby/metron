// RISC-V SiMPLE SV -- Toplevel
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef TOPLEVEL_H
`define TOPLEVEL_H

`include "config.sv"
`include "constants.sv"
`include "example_data_memory_bus.sv"
`include "example_text_memory_bus.sv"
`include "metron/metron_tools.sv"
`include "riscv_core.sv"

module toplevel (
  // global clock
  input logic clock,
  // input signals
  input logic reset,
  // output signals
  output logic[31:0] bus_read_data,
  output logic[31:0] bus_address,
  output logic[31:0] bus_write_data,
  output logic[3:0] bus_byte_enable,
  output logic bus_read_enable,
  output logic bus_write_enable,
  output logic[31:0] inst,
  output logic[31:0] pc,
  // tock() ports
  input int tock_bogus_param
);
 /*public:*/


  parameter /*const*/ text_filename = "";
  parameter /*const*/ data_filename = "";
  initial begin
  end

  //----------------------------------------

  always_comb begin : tock
    core_reset = reset;
    /*core.tock_pc();*/

    text_memory_bus_address = core_pc;
    /*text_memory_bus.tock_read_data();*/

    core_inst = text_memory_bus_read_data;
    /*core.tock_execute();*/

    data_memory_bus_address = core_bus_address;
    data_memory_bus_read_enable = core_bus_read_enable;
    data_memory_bus_write_enable = core_bus_write_enable;
    data_memory_bus_byte_enable = core_bus_byte_enable;
    data_memory_bus_write_data = core_bus_write_data;
    /*data_memory_bus.tock();*/

    core_bus_read_data = data_memory_bus_read_data;
    /*core.tock_writeback();*/

    //----------

    bus_read_data = data_memory_bus_read_data;
    bus_address = core_bus_address;
    bus_write_data = core_bus_write_data;
    bus_byte_enable = core_bus_byte_enable;
    bus_read_enable = core_bus_read_enable;
    bus_write_enable = core_bus_write_enable;
    inst = text_memory_bus_read_data;
    pc = core_pc;
  end

  //----------------------------------------

 /*private:*/
  riscv_core core(
    // Global clock
    .clock(clock),
    // Input signals
    .reset(core_reset),
    .bus_read_data(core_bus_read_data),
    .inst(core_inst),
    // Output signals
    .bus_address(core_bus_address),
    .bus_write_data(core_bus_write_data),
    .bus_byte_enable(core_bus_byte_enable),
    .bus_read_enable(core_bus_read_enable),
    .bus_write_enable(core_bus_write_enable),
    .pc(core_pc)
  );
  logic core_reset;
  logic[31:0] core_bus_read_data;
  logic[31:0] core_inst;
  logic[31:0] core_bus_address;
  logic[31:0] core_bus_write_data;
  logic[3:0] core_bus_byte_enable;
  logic core_bus_read_enable;
  logic core_bus_write_enable;
  logic[31:0] core_pc;

  example_text_memory_bus #(
    // Constructor Parameters
    .filename(text_filename)
  ) text_memory_bus(
    // Input signals
    .address(text_memory_bus_address),
    // Output signals
    .read_data(text_memory_bus_read_data)
  );
  logic[31:0] text_memory_bus_address;
  logic[31:0] text_memory_bus_read_data;
  example_data_memory_bus #(
    // Constructor Parameters
    .filename(data_filename)
  ) data_memory_bus(
    // Global clock
    .clock(clock),
    // Input signals
    .address(data_memory_bus_address),
    .write_data(data_memory_bus_write_data),
    .byte_enable(data_memory_bus_byte_enable),
    .read_enable(data_memory_bus_read_enable),
    .write_enable(data_memory_bus_write_enable),
    // Output signals
    .read_data(data_memory_bus_read_data)
  );
  logic[31:0] data_memory_bus_address;
  logic[31:0] data_memory_bus_write_data;
  logic[3:0] data_memory_bus_byte_enable;
  logic data_memory_bus_read_enable;
  logic data_memory_bus_write_enable;
  logic[31:0] data_memory_bus_read_data;
endmodule

`endif // TOPLEVEL_H
