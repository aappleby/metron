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
 /*public:*/
  /*logic<32> result;*/
  /*logic<1> result_equal_zero;*/

`ifdef M_MODULE
 /*private:*/
  logic[63:0] signed_multiplication;
  logic[63:0] unsigned_multiplication;
  logic[63:0] signed_unsigned_multiplication;

 /*public:*/
`endif

  always_comb begin : tock
    import rv_constants::*;

    case (alu_function) 
      /*case*/ ALU_ADD:
        result = operand_a + operand_b;
        /*break;*/
      /*case*/ ALU_SUB:
        result = operand_a - operand_b;
        /*break;*/
      /*case*/ ALU_SLL:
        result = operand_a << 5'(operand_b);
        /*break;*/
      /*case*/ ALU_SRL:
        result = operand_a >> 5'(operand_b);
        /*break;*/
      /*case*/ ALU_SRA:
        result = ($signed(operand_a) >>> 5'(operand_b));
        /*break;*/
      /*case*/ ALU_SEQ:
        result = {31'd0, 1'(operand_a == operand_b)};
        /*break;*/
      /*case*/ ALU_SLT:
        result = {31'd0, 1'($signed(operand_a) < $signed(operand_b))};
        /*break;*/
      /*case*/ ALU_SLTU:
        result = {31'd0, 1'($unsigned(operand_a) < $unsigned(operand_b))};
        /*break;*/
      /*case*/ ALU_XOR:
        result = operand_a ^ operand_b;
        /*break;*/
      /*case*/ ALU_OR:
        result = operand_a | operand_b;
        /*break;*/
      /*case*/ ALU_AND:
        result = operand_a & operand_b;
        /*break;*/

`ifdef M_MODULE
      /*case*/ ALU_MUL:
        result = signed_multiplication[31:0];
        /*break;*/
      /*case*/ ALU_MULH:
        result = signed_multiplication[63:32];
        /*break;*/
      /*case*/ ALU_MULHSU:
        result = signed_unsigned_multiplication[63:32];
        /*break;*/
      /*case*/ ALU_MULHU:
        result = unsigned_multiplication[63:32];
        /*break;*/
      /*case*/ ALU_DIV:
        if (operand_b == ZERO)
          result = 1;
        else if ((operand_a == 32'h80000000) && (operand_b == 1))
          result = 32'h80000000;
        else
          result = operand_a / operand_b;
        /*break;*/
      /*case*/ ALU_DIVU:
        if (operand_b == ZERO)
          result = 1;
        else
          result = $unsigned(operand_a) / $unsigned(operand_b);
        /*break;*/
      /*case*/ ALU_REM:
        if (operand_b == ZERO)
          result = operand_a;
        else if ((operand_a == 32'h80000000) && (operand_b == 1))
          result = ZERO;
        else
          result = operand_a % operand_b;
        /*break;*/
      /*case*/ ALU_REMU:
        if (operand_b == ZERO)
          result = operand_a;
        else
          result = $unsigned(operand_a) % $unsigned(operand_b);
        /*break;*/
`endif

      default:
        result = 32'(ZERO);
        /*break;*/
    endcase

    result_equal_zero = (result == 32'd0);
  end

`ifdef M_MODULE
  always_comb begin : tock_mult
    signed_multiplication = operand_a * operand_b;
    unsigned_multiplication = $unsigned(operand_a) * $unsigned(operand_b);
    signed_unsigned_multiplication = $signed(operand_a) * $unsigned(operand_b);
  end
`endif
endmodule;

`endif  // RVSIMPLE_ALU_H
