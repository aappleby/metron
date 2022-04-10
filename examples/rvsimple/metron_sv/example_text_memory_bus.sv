// RISC-V SiMPLE SV -- program text memory bus
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_EXAMPLE_TEXT_MEMORY_BUS_H
`define RVSIMPLE_EXAMPLE_TEXT_MEMORY_BUS_H

`include "config.sv"
`include "constants.sv"
`include "example_text_memory.sv"
`include "metron_tools.sv"

module example_text_memory_bus
(
  input logic clock,
  input logic[31:0] address,
  output logic[31:0] read_data
);
 /*public:*/
  /*logic<32> address;*/
  /*logic<32> read_data;*/

 /*private:*/
  example_text_memory text_memory(
    // Inputs
    .clock(clock),
    .address(text_memory_address), 
    // Outputs
    .q(text_memory_q)
  );
  logic[rv_config::TEXT_BITS - 2-1:0] text_memory_address;
  logic[31:0] text_memory_q;


 /*public:*/
  always_comb begin /*tock_read_data*/
    logic[31:0] fetched;
    text_memory_address = address[rv_config::TEXT_BITS - 2+1:2];
    /*text_memory.tock_q()*/;
    fetched = text_memory_q;
    read_data =
      (address >= rv_config::TEXT_BEGIN) && (rv_config::TEXT_END >= address)
      ? fetched
      : 32'bx;
  end
endmodule;

`endif  // RVSIMPLE_EXAMPLE_TEXT_MEMORY_BUS_H

