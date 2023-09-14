// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef MULTIPLEXER2_H
`define MULTIPLEXER2_H

`include "config.sv"
`include "constants.sv"
`include "metron/tools/metron_tools.sv"

module multiplexer2 (
  // input signals
  input logic[WIDTH-1:0] in0,
  input logic[WIDTH-1:0] in1,
  input logic sel,
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
      default:
        out = 'x;
    endcase
  end
endmodule

`endif // MULTIPLEXER2_H
