// RISC-V SiMPLE SV -- ALU module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_ALU_H
`define RVSIMPLE_ALU_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module alu
(
  input logic clock,
  input logic[4:0] alu_function,
  input logic[31:0] operand_a,
  input logic[31:0] operand_b,
  output logic[31:0] result,
  output logic result_equal_zero
);

  always_comb begin /*tock*/
    import rv_constants::*;

    // clang-format off
    case (alu_function)
      ALU_ADD:  result = operand_a + operand_b;
      ALU_SUB:  result = operand_a - operand_b;
      ALU_SLL:  result = operand_a << 5'(operand_b);
      ALU_SRL:  result = operand_a >> 5'(operand_b);
      ALU_SRA:  result = ($signed(operand_a) >>> 5'(operand_b));
      ALU_SEQ:  result = {31'b0, 1'(operand_a == operand_b)};
      ALU_SLT:  result = {31'b0, 1'($signed(operand_a) < $signed(operand_b))};
      ALU_SLTU: result = {31'b0, 1'($unsigned(operand_a) < $unsigned(operand_b))};
      ALU_XOR:  result = operand_a ^ operand_b;
      ALU_OR:   result = operand_a | operand_b;
      ALU_AND:  result = operand_a & operand_b;
      default:       result = 32'(ZERO);
    endcase
    // clang-format on

    result_equal_zero = (result == 32'd0);
  end
endmodule;

`endif  // RVSIMPLE_ALU_H

