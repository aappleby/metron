// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef MULTIPLEXER4_H
`define MULTIPLEXER4_H

`include "config.sv"
`include "constants.sv"
`include "metron/tools/metron_tools.sv"

module multiplexer4 (
  // input signals
  input logic[WIDTH-1:0] in0,
  input logic[WIDTH-1:0] in1,
  input logic[WIDTH-1:0] in2,
  input logic[WIDTH-1:0] in3,
  input logic[1:0] sel,
  // output signals
  output logic[WIDTH-1:0] out
);
  parameter WIDTH = 32;

 /*public:*/

  always_comb begin : tock
    case (sel)
      0:
        out = in0;
      1:
        out = in1;
      2:
        out = in2;
      3:
        out = in3;
      default:
        out = 'x;
    endcase
  end
endmodule

`endif // MULTIPLEXER4_H
