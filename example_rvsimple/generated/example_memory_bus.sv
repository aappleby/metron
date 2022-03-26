// RISC-V SiMPLE SV -- combined text/data memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_EXAMPLE_MEMORY_BUS_H
`define RVSIMPLE_EXAMPLE_MEMORY_BUS_H

`include "config.sv"
`include "constants.sv"
`include "example_data_memory.sv"
`include "example_text_memory.sv"
`include "metron_tools.sv"

module example_memory_bus
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
  /*logic<32> read_data;*/

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

  example_text_memory text_memory(
    // Inputs
    .clock(clock),
    .address(text_memory_address), 
    // Outputs
    .q(text_memory_q)
  );
  logic[TEXT_BITS - 2-1:0] text_memory_address;
  logic[31:0] text_memory_q;


  always_ff @(posedge clock) begin : tick
    /*data_memory.tick(
        bx<DATA_BITS - 2>(address, 2),
        write_enable && address >= DATA_BEGIN && address <= DATA_END,
        byte_enable, write_data);*/
    data_memory_address = address[DATA_BITS - 2+1:2];
    data_memory_wren = write_enable && address >= DATA_BEGIN && address <= DATA_END;
    data_memory_byteena = byte_enable;
    data_memory_data = write_data;
    
  end

  always_comb begin : tock
    logic[31:0] text_fetched;
    logic[31:0] data_fetched;
    logic is_data_memory;
    logic is_text_memory;
    /*data_memory.tock(bx<DATA_BITS - 2>(address, 2));*/
    data_memory_address = address[DATA_BITS - 2+1:2];
    
    /*text_memory.tock(bx<TEXT_BITS - 2>(address, 2));*/
    text_memory_address = address[TEXT_BITS - 2+1:2];
    

    text_fetched = text_memory_q;
    data_fetched = data_memory_q;

    is_data_memory = address >= DATA_BEGIN && address <= DATA_END;
    is_text_memory = address >= TEXT_BEGIN && address <= TEXT_END;

    if (read_enable) begin
      if (is_text_memory)
        read_data = text_memory_q;
      else if (is_data_memory)
        read_data = data_memory_q;
      else
        read_data = 32'x;
    end else begin
      read_data = 32'x;
    end
  end
endmodule;

`endif  // RVSIMPLE_EXAMPLE_MEMORY_BUS_H
