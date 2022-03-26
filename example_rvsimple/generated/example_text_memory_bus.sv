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
  /*logic<32> read_data;*/

 /*private:*/
  example_text_memory text_memory(
    // Inputs
    .clock(clock),
    .address(address[TEXT_BITS - 2+1:2]), 
    // Outputs
    .q(text_memory_q)
  );
  logic[31:0] text_memory_q;


 /*public:*/
  always_comb begin : tock
    logic[31:0] fetched;
    /*text_memory.tock(bx<TEXT_BITS - 2>(address, 2));*/
    fetched = text_memory_q;

    read_data =
        address >= TEXT_BEGIN && address <= TEXT_END
        ? fetched
        : 32'x;
  end

  initial begin : init /*text_memory.init();*/ end
endmodule;

`endif  // RVSIMPLE_EXAMPLE_TEXT_MEMORY_BUS_H

