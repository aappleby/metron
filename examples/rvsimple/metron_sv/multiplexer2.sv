// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef MULTIPLEXER2_H
`define MULTIPLEXER2_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

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
         begin out = in0;
        /*break;*/ end
      1:
         begin out = in1;
        /*break;*/ end
      default:
        out = 'x;
        /*break;*/
    endcase
  end
endmodule

`endif // MULTIPLEXER2_H
