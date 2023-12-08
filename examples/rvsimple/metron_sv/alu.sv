// RISC-V SiMPLE SV -- ALU module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef ALU_H
`define ALU_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module alu (
);
 /*public:*/

  always_comb begin : tock
    import rv_constants::*;

    // clang-format off


    // clang-format off
    case (alu_function)
      ALU_ADD:   begin result = operand_a + operand_b; /*break;*/ end
      ALU_SUB:   begin result = operand_a - operand_b; /*break;*/ end
      ALU_SLL:   begin result = operand_a << 5'(operand_b); /*break;*/ end
      ALU_SRL:   begin result = operand_a >> 5'(operand_b); /*break;*/ end
      ALU_SRA:   begin result = ($signed(operand_a) >>> 5'(operand_b)); /*break;*/ end
      ALU_SEQ:   begin result = {31'b0, 1'(operand_a == operand_b)}; /*break;*/ end
      ALU_SLT:   begin result = {31'b0, 1'($signed(operand_a) < $signed(operand_b))}; /*break;*/ end
      ALU_SLTU:  begin result = {31'b0, 1'($unsigned(operand_a) < $unsigned(operand_b))}; /*break;*/ end
      ALU_XOR:   begin result = operand_a ^ operand_b; /*break;*/ end
      ALU_OR:    begin result = operand_a | operand_b; /*break;*/ end
      ALU_AND:   begin result = operand_a & operand_b; /*break;*/ end
      default:       result = 32'(ZERO); /*break;*/
    endcase
    // clang-format on

    result_equal_zero = (result == 32'd0);
  end
endmodule

`endif // ALU_H
