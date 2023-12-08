// RISC-V SiMPLE SV -- instruction decoder
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef INSTRUCTION_DECODER_H
`define INSTRUCTION_DECODER_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module instruction_decoder (
);
 /*public:*/

  always_comb begin : tock
    inst_opcode = 7'(inst);
    inst_funct3 = inst[14:12];
    inst_funct7 = inst[31:25];
    inst_rd = inst[11:7];
    inst_rs1 = inst[19:15];
    inst_rs2 = inst[24:20];
  end
endmodule

`endif // INSTRUCTION_DECODER_H
