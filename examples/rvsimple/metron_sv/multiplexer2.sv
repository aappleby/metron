// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef MULTIPLEXER2_H
`define MULTIPLEXER2_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module multiplexer2
#(parameter int WIDTH = 32)
(
  input logic[WIDTH-1:0] in0,
  input logic[WIDTH-1:0] in1,
  input logic sel,
  output logic[WIDTH-1:0] out
);
 /*public:*/

  task automatic tock();
    case (sel)
      0:
        out = in0;
      1:
        out = in1;
      default:
        out = WIDTH'(1'bx);
    endcase
  endtask
  always_comb tock();
endmodule

`endif // MULTIPLEXER2_H
