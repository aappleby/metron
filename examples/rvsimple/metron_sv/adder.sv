// RISC-V SiMPLE SV -- adder module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef ADDER_H
`define ADDER_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module adder
#(parameter int WIDTH = 32)
(
  input logic[WIDTH-1:0] operand_a,
  input logic[WIDTH-1:0] operand_b,
  output logic[WIDTH-1:0] result
);
 /*public:*/

  function tock();  result = operand_a + operand_b; endfunction
  always_comb tock();
endmodule

`endif // ADDER_H
