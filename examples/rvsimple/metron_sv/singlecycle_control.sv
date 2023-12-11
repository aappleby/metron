// RISC-V SiMPLE SV -- single-cycle controller
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef SINGLECYCLE_CONTROL_H
`define SINGLECYCLE_CONTROL_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module singlecycle_control (
  // input signals
  input logic[6:0] inst_opcode,
  input logic take_branch,
  // output signals
  output logic pc_write_enable,
  output logic regfile_write_enable,
  output logic alu_operand_a_select,
  output logic alu_operand_b_select,
  output logic[1:0] alu_op_type,
  output logic data_mem_read_enable,
  output logic data_mem_write_enable,
  output logic[2:0] reg_writeback_select,
  output logic[1:0] next_pc_select
);
 /*public:*/

  always_comb begin : tock_next_pc_select
    import rv_constants::*;
    // clang-format off

    // clang-format off
    case (inst_opcode)
      OPCODE_BRANCH:  begin next_pc_select = take_branch ? CTL_PC_PC_IMM : CTL_PC_PC4; /*break;*/ end
      OPCODE_JALR:    begin next_pc_select = CTL_PC_RS1_IMM; /*break;*/ end
      OPCODE_JAL:     begin next_pc_select = CTL_PC_PC_IMM; /*break;*/ end
      default:            next_pc_select = CTL_PC_PC4; /*break;*/
    endcase
    // clang-format on
  end

  always_comb begin : tock_pc_write_enable  pc_write_enable = 1'b1; end

  always_comb begin : tock_regfile_write_enable
    import rv_constants::*;
    // clang-format off

    // clang-format off
    case (inst_opcode)
      OPCODE_MISC_MEM:  begin regfile_write_enable = 0; /*break;*/ end
      OPCODE_STORE:     begin regfile_write_enable = 0; /*break;*/ end
      OPCODE_BRANCH:    begin regfile_write_enable = 0; /*break;*/ end
      OPCODE_LOAD:      begin regfile_write_enable = 1; /*break;*/ end
      OPCODE_OP_IMM:    begin regfile_write_enable = 1; /*break;*/ end
      OPCODE_AUIPC:     begin regfile_write_enable = 1; /*break;*/ end
      OPCODE_OP:        begin regfile_write_enable = 1; /*break;*/ end
      OPCODE_LUI:       begin regfile_write_enable = 1; /*break;*/ end
      OPCODE_JALR:      begin regfile_write_enable = 1; /*break;*/ end
      OPCODE_JAL:       begin regfile_write_enable = 1; /*break;*/ end
      default:              regfile_write_enable = 'x; /*break;*/
    endcase
    // clang-format on
  end

  always_comb begin : tock_alu_operand_a_select
    import rv_constants::*;

    // clang-format off


    // clang-format off
    case (inst_opcode)
      OPCODE_AUIPC:     begin alu_operand_a_select = CTL_ALU_A_PC; /*break;*/ end
      OPCODE_JAL:       begin alu_operand_a_select = CTL_ALU_A_PC; /*break;*/ end

      OPCODE_OP:        begin alu_operand_a_select = CTL_ALU_A_RS1; /*break;*/ end
      OPCODE_LUI:       begin alu_operand_a_select = CTL_ALU_A_RS1; /*break;*/ end
      OPCODE_BRANCH:    begin alu_operand_a_select = CTL_ALU_A_RS1; /*break;*/ end

      OPCODE_LOAD:      begin alu_operand_a_select = CTL_ALU_A_RS1; /*break;*/ end
      OPCODE_STORE:     begin alu_operand_a_select = CTL_ALU_A_RS1; /*break;*/ end
      OPCODE_OP_IMM:    begin alu_operand_a_select = CTL_ALU_A_RS1; /*break;*/ end
      OPCODE_JALR:      begin alu_operand_a_select = CTL_ALU_A_RS1; /*break;*/ end
      default:              alu_operand_a_select = 'x; /*break;*/
    endcase
    // clang-format on
  end

  always_comb begin : tock_alu_operand_b_select
    import rv_constants::*;

    // clang-format off


    // clang-format off
    case (inst_opcode)
      OPCODE_AUIPC:     begin alu_operand_b_select = CTL_ALU_B_IMM; /*break;*/ end
      OPCODE_JAL:       begin alu_operand_b_select = CTL_ALU_B_IMM; /*break;*/ end

      OPCODE_OP:        begin alu_operand_b_select = CTL_ALU_B_RS2; /*break;*/ end
      OPCODE_LUI:       begin alu_operand_b_select = CTL_ALU_B_RS2; /*break;*/ end
      OPCODE_BRANCH:    begin alu_operand_b_select = CTL_ALU_B_RS2; /*break;*/ end

      OPCODE_LOAD:      begin alu_operand_b_select = CTL_ALU_B_IMM; /*break;*/ end
      OPCODE_STORE:     begin alu_operand_b_select = CTL_ALU_B_IMM; /*break;*/ end
      OPCODE_OP_IMM:    begin alu_operand_b_select = CTL_ALU_B_IMM; /*break;*/ end
      OPCODE_JALR:      begin alu_operand_b_select = CTL_ALU_B_IMM; /*break;*/ end
      default:              alu_operand_b_select = 'x; /*break;*/
    endcase
    // clang-format on
  end

  always_comb begin : tock_alu_op_type
    import rv_constants::*;

    // clang-format off


    // clang-format off
    case (inst_opcode)
      OPCODE_AUIPC:     begin alu_op_type = CTL_ALU_ADD; /*break;*/ end
      OPCODE_JAL:       begin alu_op_type = CTL_ALU_ADD; /*break;*/ end

      OPCODE_OP:        begin alu_op_type = CTL_ALU_OP; /*break;*/ end
      OPCODE_BRANCH:    begin alu_op_type = CTL_ALU_BRANCH; /*break;*/ end

      OPCODE_LOAD:      begin alu_op_type = CTL_ALU_ADD; /*break;*/ end
      OPCODE_STORE:     begin alu_op_type = CTL_ALU_ADD; /*break;*/ end
      OPCODE_OP_IMM:    begin alu_op_type = CTL_ALU_OP_IMM; /*break;*/ end
      OPCODE_JALR:      begin alu_op_type = CTL_ALU_ADD; /*break;*/ end
      default:              alu_op_type = 'x; /*break;*/
    endcase
    // clang-format on
  end

  always_comb begin : tock_data_mem_read_enable
    import rv_constants::*;

    data_mem_read_enable = inst_opcode == OPCODE_LOAD;
  end

  always_comb begin : tock_data_mem_write_enable
    import rv_constants::*;

    data_mem_write_enable = inst_opcode == OPCODE_STORE;
  end

  always_comb begin : tock_reg_writeback_select
    import rv_constants::*;

    // clang-format off


    // clang-format off
    case (inst_opcode)
      OPCODE_OP_IMM:    begin reg_writeback_select = CTL_WRITEBACK_ALU; /*break;*/ end
      OPCODE_AUIPC:     begin reg_writeback_select = CTL_WRITEBACK_ALU; /*break;*/ end
      OPCODE_OP:        begin reg_writeback_select = CTL_WRITEBACK_ALU; /*break;*/ end
      OPCODE_LUI:       begin reg_writeback_select = CTL_WRITEBACK_IMM; /*break;*/ end
      OPCODE_JALR:      begin reg_writeback_select = CTL_WRITEBACK_PC4; /*break;*/ end
      OPCODE_JAL:       begin reg_writeback_select = CTL_WRITEBACK_PC4; /*break;*/ end
      OPCODE_LOAD:      begin reg_writeback_select = CTL_WRITEBACK_DATA; /*break;*/ end
      default:              reg_writeback_select = 'x; /*break;*/
    endcase
    // clang-format on
  end
endmodule

`endif // SINGLECYCLE_CONTROL_H
