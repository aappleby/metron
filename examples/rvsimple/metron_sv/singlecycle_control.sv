// RISC-V SiMPLE SV -- single-cycle controller
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_SINGLECYCLE_CONTROL_H
`define RVSIMPLE_SINGLECYCLE_CONTROL_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module singlecycle_control
(
  input logic clock,
  input logic[6:0] inst_opcode,
  input logic take_branch,
  output logic[1:0] next_pc_select,
  output logic pc_write_enable,
  output logic regfile_write_enable,
  output logic alu_operand_a_select,
  output logic alu_operand_b_select,
  output logic[1:0] alu_op_type2,
  output logic data_mem_read_enable,
  output logic data_mem_write_enable,
  output logic[2:0] reg_writeback_select
);
 /*public:*/

  always_comb begin
    logic[1:0] result;
    import rv_constants::*;
    /*logic<2> result;*/
    case (inst_opcode) 
      /*case*/ OPCODE_BRANCH: result = take_branch ? CTL_PC_PC_IMM : CTL_PC_PC4; /*break;*/
      /*case*/ OPCODE_JALR:   result = CTL_PC_RS1_IMM; /*break;*/
      /*case*/ OPCODE_JAL:    result = CTL_PC_PC_IMM; /*break;*/
      default:            result = CTL_PC_PC4; /*break;*/
    endcase
    next_pc_select = result;
  end

  always_comb begin
    pc_write_enable = 1'b1;
  end

  always_comb begin
    logic result;
    import rv_constants::*;
    /*logic<1> result;*/
    case (inst_opcode) 
      /*case*/ OPCODE_MISC_MEM: result = 0; /*break;*/
      /*case*/ OPCODE_STORE:    result = 0; /*break;*/
      /*case*/ OPCODE_BRANCH:   result = 0; /*break;*/
      /*case*/ OPCODE_LOAD:     result = 1; /*break;*/
      /*case*/ OPCODE_OP_IMM:   result = 1; /*break;*/
      /*case*/ OPCODE_AUIPC:    result = 1; /*break;*/
      /*case*/ OPCODE_OP:       result = 1; /*break;*/
      /*case*/ OPCODE_LUI:      result = 1; /*break;*/
      /*case*/ OPCODE_JALR:     result = 1; /*break;*/
      /*case*/ OPCODE_JAL:      result = 1; /*break;*/
      default:              result = 1'bx; /*break;*/
    endcase
    regfile_write_enable = result;
  end

  always_comb begin
    logic result;
    import rv_constants::*;

    /*logic<1> result;*/
    case (inst_opcode) 
      /*case*/ OPCODE_AUIPC:    result = CTL_ALU_A_PC; /*break;*/
      /*case*/ OPCODE_JAL:      result = CTL_ALU_A_PC; /*break;*/

      /*case*/ OPCODE_OP:       result = CTL_ALU_A_RS1; /*break;*/
      /*case*/ OPCODE_LUI:      result = CTL_ALU_A_RS1; /*break;*/
      /*case*/ OPCODE_BRANCH:   result = CTL_ALU_A_RS1; /*break;*/

      /*case*/ OPCODE_LOAD:     result = CTL_ALU_A_RS1; /*break;*/
      /*case*/ OPCODE_STORE:    result = CTL_ALU_A_RS1; /*break;*/
      /*case*/ OPCODE_OP_IMM:   result = CTL_ALU_A_RS1; /*break;*/
      /*case*/ OPCODE_JALR:     result = CTL_ALU_A_RS1; /*break;*/
      default:              result = 1'bx; /*break;*/
    endcase
    alu_operand_a_select = result;
  end

  always_comb begin
    logic result;
    import rv_constants::*;
    /*logic<1> result;*/
    case (inst_opcode) 
      /*case*/ OPCODE_AUIPC:    result = CTL_ALU_B_IMM; /*break;*/
      /*case*/ OPCODE_JAL:      result = CTL_ALU_B_IMM; /*break;*/

      /*case*/ OPCODE_OP:       result = CTL_ALU_B_RS2; /*break;*/
      /*case*/ OPCODE_LUI:      result = CTL_ALU_B_RS2; /*break;*/
      /*case*/ OPCODE_BRANCH:   result = CTL_ALU_B_RS2; /*break;*/

      /*case*/ OPCODE_LOAD:     result = CTL_ALU_B_IMM; /*break;*/
      /*case*/ OPCODE_STORE:    result = CTL_ALU_B_IMM; /*break;*/
      /*case*/ OPCODE_OP_IMM:   result = CTL_ALU_B_IMM; /*break;*/
      /*case*/ OPCODE_JALR:     result = CTL_ALU_B_IMM; /*break;*/
      default:              result = 1'bx; /*break;*/
    endcase
    alu_operand_b_select = result;
  end

  always_comb begin
    logic[1:0] result;
    import rv_constants::*;
    /*logic<2> result;*/
    case (inst_opcode) 
      /*case*/ OPCODE_AUIPC:    result = CTL_ALU_ADD; /*break;*/
      /*case*/ OPCODE_JAL:      result = CTL_ALU_ADD; /*break;*/

      /*case*/ OPCODE_OP:       result = CTL_ALU_OP; /*break;*/
      /*case*/ OPCODE_BRANCH:   result = CTL_ALU_BRANCH; /*break;*/

      /*case*/ OPCODE_LOAD:     result = CTL_ALU_ADD; /*break;*/
      /*case*/ OPCODE_STORE:    result = CTL_ALU_ADD; /*break;*/
      /*case*/ OPCODE_OP_IMM:   result = CTL_ALU_OP_IMM; /*break;*/
      /*case*/ OPCODE_JALR:     result = CTL_ALU_ADD; /*break;*/
      default:              result = 2'bx; /*break;*/
    endcase
    alu_op_type2 = result;
  end

  always_comb begin
    import rv_constants::*;
    data_mem_read_enable = inst_opcode == OPCODE_LOAD;
  end

  always_comb begin
    import rv_constants::*;
    data_mem_write_enable = inst_opcode == OPCODE_STORE;
  end

  always_comb begin
    logic[2:0] result;
    import rv_constants::*;
    /*logic<3> result;*/
    case (inst_opcode) 
      /*case*/ OPCODE_OP_IMM:   result = CTL_WRITEBACK_ALU; /*break;*/
      /*case*/ OPCODE_AUIPC:    result = CTL_WRITEBACK_ALU; /*break;*/
      /*case*/ OPCODE_OP:       result = CTL_WRITEBACK_ALU; /*break;*/
      /*case*/ OPCODE_LUI:      result = CTL_WRITEBACK_IMM; /*break;*/
      /*case*/ OPCODE_JALR:     result = CTL_WRITEBACK_PC4; /*break;*/
      /*case*/ OPCODE_JAL:      result = CTL_WRITEBACK_PC4; /*break;*/
      /*case*/ OPCODE_LOAD:     result = CTL_WRITEBACK_DATA; /*break;*/
      default:              result = 3'bx; /*break;*/
    endcase
    reg_writeback_select = result;
  end
endmodule;

`endif  // RVSIMPLE_SINGLECYCLE_CONTROL_H

