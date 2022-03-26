// RISC-V SiMPLE SV -- instruction decoder
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_INSTRUCTION_DECODER_H
`define RVSIMPLE_INSTRUCTION_DECODER_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module instruction_decoder
(
  input logic clock,
  input logic[31:0] inst,
  output logic[6:0] inst_opcode2,
  output logic[2:0] inst_funct32,
  output logic[6:0] inst_funct72,
  output logic[4:0] inst_rd2,
  output logic[4:0] inst_rs12,
  output logic[4:0] inst_rs22
);
 /*public:*/
  always_comb begin inst_opcode2 = inst[6:0]; end
  always_comb begin inst_funct32 = inst[14:12]; end
  always_comb begin inst_funct72 = inst[31:25]; end
  always_comb begin inst_rd2 = inst[11:7]; end
  always_comb begin inst_rs12 = inst[19:15]; end
  always_comb begin inst_rs22 = inst[24:20]; end

  always_comb begin : tock
    //inst_opcode = b7(inst, 0);
    //inst_funct3 = b3(inst, 12);
    //inst_funct7 = b7(inst, 25);
    //inst_rd = b5(inst, 7);
    //inst_rs1 = b5(inst, 15);
    //inst_rs2 = b5(inst, 20);
  end
endmodule;

`endif  // RVSIMPLE_INSTRUCTION_DECODER_H

