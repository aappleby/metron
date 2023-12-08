// RISC-V SiMPLE SV -- immediate generator
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef IMMEDIATE_GENERATOR_H
`define IMMEDIATE_GENERATOR_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module immediate_generator (
);
 /*public:*/

  // clang-format off
  // Immediate format
  //       31.............30........20.19........12.11.....11.10.........5.4..........1.0.....0
  // I = { {21{inst[31]}},                                     inst[30:25], inst[24:20]         };
  // S = { {21{inst[31]}},                                     inst[30:25], inst[11:7]          };
  // B = { {20{inst[31]}}, inst[7],                            inst[30:25], inst[11:8],   1'b0  };
  // U = { {1{inst[31]}},  inst[30:20], inst[19:12],                                      12'b0 };
  // J = { {12{inst[31]}},              inst[19:12], inst[20], inst[30:25], inst[24:21],  1'b0  };
  // clang-format on

  always_comb begin : tock
    import rv_constants::*;
    // clang-format off

    // clang-format off
    immediate = 32'b0;
    case (7'(inst))  // Opcode
      OPCODE_LOAD,
      OPCODE_LOAD_FP,
      OPCODE_OP_IMM,
      OPCODE_JALR: // I-type immediate
         begin immediate = {{21 {inst[31]}}, inst[30:25], inst[24:20]}; /*break;*/ end
      OPCODE_STORE_FP,
      OPCODE_STORE: // S-type immediate
         begin immediate = {{21 {inst[31]}}, inst[30:25], inst[11:7]}; /*break;*/ end
      OPCODE_BRANCH: // B-type immediate
         begin immediate = {{20 {inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0}; /*break;*/ end
      OPCODE_AUIPC,
      OPCODE_LUI: // U-type immediate
         begin immediate = {inst[31], inst[30:20], inst[19:12], 12'b0}; /*break;*/ end
      OPCODE_JAL: // J-type immediate
         begin immediate = {{12 {inst[31]}}, inst[19:12], inst[20], inst[30:25], inst[24:21], 1'b0}; /*break;*/ end
      default: immediate = 32'b0; /*break;*/
    endcase
    // clang-format on
  end
endmodule

`endif // IMMEDIATE_GENERATOR_H
