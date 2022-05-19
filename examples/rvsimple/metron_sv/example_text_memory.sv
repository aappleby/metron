// RISC-V SiMPLE SV -- text memory model
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef EXAMPLE_TEXT_MEMORY_H
`define EXAMPLE_TEXT_MEMORY_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module example_text_memory
(
  input logic[rv_config::TEXT_BITS - 2-1:0] address,
  output logic[31:0] q
);
 /*public:*/
  initial begin
    string s;
    $value$plusargs("text_file=%s", s);
    $readmemh(s, mem);
  end

  always_comb begin : tock_q q = mem[address]; end

 /*private:*/
  logic[31:0] mem[2**(rv_config::TEXT_BITS - 2)];
endmodule

`endif // EXAMPLE_TEXT_MEMORY_H
