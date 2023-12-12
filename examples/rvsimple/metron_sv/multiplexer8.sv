// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef MULTIPLEXER8_H
`define MULTIPLEXER8_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module multiplexer8 (
  // input signals
  input logic[WIDTH-1:0] in0,
  input logic[WIDTH-1:0] in1,
  input logic[WIDTH-1:0] in2,
  input logic[WIDTH-1:0] in3,
  input logic[WIDTH-1:0] in4,
  input logic[WIDTH-1:0] in5,
  input logic[WIDTH-1:0] in6,
  input logic[WIDTH-1:0] in7,
  input logic[2:0] sel,
  // output signals
  output logic[WIDTH-1:0] out
);
  parameter WIDTH = 32;
 /*public:*/

  always_comb begin : tock
    // clang-format off
    case (sel)
      0:  out = in0; /*break;*/
      1:  out = in1; /*break;*/
      2:  out = in2; /*break;*/
      3:  out = in3; /*break;*/
      4:  out = in4; /*break;*/
      5:  out = in5; /*break;*/
      6:  out = in6; /*break;*/
      7:  out = in7; /*break;*/
      default: out = 'x; /*break;*/
    endcase
    // clang-format on
  end
endmodule

`endif // MULTIPLEXER8_H
