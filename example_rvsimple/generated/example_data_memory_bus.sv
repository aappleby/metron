// RISC-V SiMPLE SV -- data memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_EXAMPLE_DATA_MEMORY_BUS_H
`define RVSIMPLE_EXAMPLE_DATA_MEMORY_BUS_H

`include "config.sv"
`include "constants.sv"
`include "example_data_memory.sv"
`include "metron_tools.sv"

module example_data_memory_bus
(
  input logic clock,
  input logic[31:0] address,
  input logic write_enable,
  input logic[3:0] byte_enable,
  input logic[31:0] write_data,
  input logic read_enable,
  output logic[31:0] read_data
);
 /*public:*/
  initial begin : init /*data_memory.init();*/ end

  always_comb begin : tock_data_memory
    data_memory_address = address[DATA_BITS - 2+1:2];
    data_memory_wren = 1'(write_enable && address >= DATA_BEGIN && DATA_END >= address);
    data_memory_byteena = byte_enable;
    data_memory_data = write_data;
    /*data_memory.tick(
        bx<DATA_BITS - 2>(address, 2),
        b1(write_enable && address >= DATA_BEGIN && DATA_END >= address),
        byte_enable, write_data);*/
  end

  example_data_memory data_memory(
    // Inputs
    .clock(clock),
    .address(data_memory_address), 
    .wren(data_memory_wren), 
    .byteena(data_memory_byteena), 
    .data(data_memory_data), 
    // Outputs
    .q(data_memory_q)
  );
  logic[DATA_BITS - 2-1:0] data_memory_address;
  logic data_memory_wren;
  logic[3:0] data_memory_byteena;
  logic[31:0] data_memory_data;
  logic[31:0] data_memory_q;


  always_comb begin
    logic[31:0] fetched;
    logic is_data_memory;
    data_memory_address = address[DATA_BITS - 2+1:2];
    fetched = data_memory_q;
    is_data_memory = address >= DATA_BEGIN && DATA_END >= address;
    read_data = read_enable && is_data_memory ? fetched : 32'x;
  end

endmodule;

`endif  // RVSIMPLE_EXAMPLE_DATA_MEMORY_BUS_H

