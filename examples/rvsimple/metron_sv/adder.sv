// RISC-V SiMPLE SV -- adder module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef ADDER_H
`define ADDER_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module adder
#(
  parameter WIDTH = 32
)
(
  // input signals
  input logic[WIDTH-1:0] operand_a,
  input logic[WIDTH-1:0] operand_b,
  // output signals
  output logic[WIDTH-1:0] result
);
 /*public:*/

  always_comb begin : tock  result = operand_a + operand_b; end
endmodule

`endif // ADDER_H
