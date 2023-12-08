// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef MULTIPLEXER4_H
`define MULTIPLEXER4_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module multiplexer4 (
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
      2:
         begin out = in2;
        /*break;*/ end
      3:
         begin out = in3;
        /*break;*/ end
      default:
        out = 'x;
        /*break;*/
    endcase
  end
endmodule

`endif // MULTIPLEXER4_H
