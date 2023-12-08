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
);
  parameter WIDTH = 32;
 /*public:*/

  always_comb begin : tock
    // clang-format off
    case (sel)
      0:   begin out = in0; /*break;*/ end
      1:   begin out = in1; /*break;*/ end
      2:   begin out = in2; /*break;*/ end
      3:   begin out = in3; /*break;*/ end
      4:   begin out = in4; /*break;*/ end
      5:   begin out = in5; /*break;*/ end
      6:   begin out = in6; /*break;*/ end
      7:   begin out = in7; /*break;*/ end
      default: out = 'x; /*break;*/
    endcase
    // clang-format on
  end
endmodule

`endif // MULTIPLEXER8_H
