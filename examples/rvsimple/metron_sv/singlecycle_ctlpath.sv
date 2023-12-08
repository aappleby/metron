// RISC-V SiMPLE SV -- control path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef SINGLECYCLE_CTLPATH_H
`define SINGLECYCLE_CTLPATH_H

`include "alu_control.sv"
`include "config.sv"
`include "constants.sv"
`include "control_transfer.sv"
`include "metron/metron_tools.sv"
`include "singlecycle_control.sv"

module singlecycle_ctlpath (
);
 /*public:*/


  always_comb begin : tock_alu_function
    control_inst_opcode = inst_opcode;
    /*control.tock_alu_op_type();*/
    alu_ctrl_alu_op_type = control_alu_op_type;
    alu_ctrl_inst_funct3 = inst_funct3;
    alu_ctrl_inst_funct7 = inst_funct7;
    /*alu_ctrl.tock_alu_function();*/
    alu_function = alu_ctrl_alu_function;
  end

  always_comb begin : tock_alu_operand_select
    /*control.tock_alu_operand_a_select();*/
    /*control.tock_alu_operand_b_select();*/
    alu_operand_a_select = control_alu_operand_a_select;
    alu_operand_b_select = control_alu_operand_b_select;
  end

  always_comb begin : tock_data_mem_enable
    /*control.tock_data_mem_read_enable();*/
    /*control.tock_data_mem_write_enable();*/
    data_mem_read_enable = control_data_mem_read_enable;
    data_mem_write_enable = control_data_mem_write_enable;
  end

  always_comb begin : tock_pc_write_enable
    /*control.tock_pc_write_enable();*/
    pc_write_enable = control_pc_write_enable;
  end

  always_comb begin : tock_regfile_write_enable
    /*control.tock_regfile_write_enable();*/
    regfile_write_enable = control_regfile_write_enable;
  end

  always_comb begin : tock_reg_writeback_select
    /*control.tock_reg_writeback_select();*/
    reg_writeback_select = control_reg_writeback_select;
  end

  always_comb begin : tock_next_pc_select
    transfer_result_equal_zero = alu_result_equal_zero;
    transfer_inst_funct3 = inst_funct3;
    /*transfer.tock_take_branch();*/
    control_take_branch = transfer_take_branch;
    /*control.tock_next_pc_select();*/
    next_pc_select = control_next_pc_select;
  end

  //----------------------------------------

 /*private:*/
  singlecycle_control control(
  );
  control_transfer transfer(
  );
  alu_control alu_ctrl(
  );
endmodule

`endif // SINGLECYCLE_CTLPATH_H
