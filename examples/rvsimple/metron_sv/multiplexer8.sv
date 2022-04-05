// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_MULTIPLEXER8_H
`define RVSIMPLE_MULTIPLEXER8_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module multiplexer8
#(parameter int WIDTH = 32)
(
  input logic clock,
  input logic[2:0] sel,
  input logic[WIDTH-1:0] in0,
  input logic[WIDTH-1:0] in1,
  input logic[WIDTH-1:0] in2,
  input logic[WIDTH-1:0] in3,
  input logic[WIDTH-1:0] in4,
  input logic[WIDTH-1:0] in5,
  input logic[WIDTH-1:0] in6,
  input logic[WIDTH-1:0] in7,
  output logic[WIDTH-1:0] out
);
 /*public:*/

  always_comb begin
    logic[WIDTH-1:0] result;
    /*logic<WIDTH> result;*/
    case (sel) 
      /*case*/ 0:  result = in0; /*break;*/
      /*case*/ 1:  result = in1; /*break;*/
      /*case*/ 2:  result = in2; /*break;*/
      /*case*/ 3:  result = in3; /*break;*/
      /*case*/ 4:  result = in3; /*break;*/
      /*case*/ 5:  result = in3; /*break;*/
      /*case*/ 6:  result = in3; /*break;*/
      /*case*/ 7:  result = in3; /*break;*/
      default: result = WIDTH'(1'bx); /*break;*/
    endcase
    out = result;
  end
endmodule

`endif  // RVSIMPLE_MULTIPLEXER2_H

