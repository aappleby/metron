// RISC-V SiMPLE SV -- single-cycle data path
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef SINGLECYCLE_DATAPATH_H
`define SINGLECYCLE_DATAPATH_H

`include "adder.sv"
`include "alu.sv"
`include "config.sv"
`include "constants.sv"
`include "immediate_generator.sv"
`include "instruction_decoder.sv"
`include "metron/metron_tools.sv"
`include "multiplexer2.sv"
`include "multiplexer4.sv"
`include "multiplexer8.sv"
`include "regfile.sv"
`include "register.sv"

module singlecycle_datapath (
);
 /*public:*/


  // control signals

 /*private:*/
  logic[31:0] rs1_data;
  logic[31:0] rs2_data;

  logic[4:0] inst_rd;
  logic[4:0] inst_rs1;
  logic[4:0] inst_rs2;

 /*public:*/
  //----------------------------------------

  always_comb begin : tock_pc  pc = program_counter_value; end

  //----------------------------------------

  always_comb begin : tock_instruction_decoder
    idec_inst = inst;
    /*idec.tock();*/

    inst_opcode = idec_inst_opcode;
    inst_funct3 = idec_inst_funct3;
    inst_funct7 = idec_inst_funct7;
    inst_rd = idec_inst_rd;
    inst_rs1 = idec_inst_rs1;
    inst_rs2 = idec_inst_rs2;
  end

  //----------------------------------------

  always_comb begin : tock_immediate_generator
    igen_inst = inst;
    /*igen.tock();*/
  end

  //----------------------------------------

  always_comb begin : tock_reg_read
    regs_rd_address = idec_inst_rd;
    regs_rs1_address = idec_inst_rs1;
    regs_rs2_address = idec_inst_rs2;
    /*regs.tock1();*/
    rs1_data = regs_rs1_data;
    rs2_data = regs_rs2_data;
  end

  always_comb begin : tock_mux_operand_a
    mux_operand_a_sel = alu_operand_a_select;
    mux_operand_a_in0 = regs_rs1_data;
    mux_operand_a_in1 = program_counter_value;
    /*mux_operand_a.tock();*/
  end

  always_comb begin : tock_mux_operand_b
    mux_operand_b_sel = alu_operand_b_select;
    mux_operand_b_in0 = regs_rs2_data;
    mux_operand_b_in1 = igen_immediate;
    /*mux_operand_b.tock();*/
  end

  always_comb begin : tock_alu
    alu_core_alu_function = alu_function;
    alu_core_operand_a = mux_operand_a_out;
    alu_core_operand_b = mux_operand_b_out;
    /*alu_core.tock();*/
    alu_result_equal_zero = alu_core_result_equal_zero;
  end

  always_comb begin : tock_adder_pc_plus_4
    adder_pc_plus_4_operand_a = 32'h00000004;
    adder_pc_plus_4_operand_b = program_counter_value;
    /*adder_pc_plus_4.tock();*/
  end

  always_comb begin : tock_adder_pc_plus_immediate
    adder_pc_plus_immediate_operand_a = program_counter_value;
    adder_pc_plus_immediate_operand_b = igen_immediate;
    /*adder_pc_plus_immediate.tock();*/
  end

  always_comb begin : tock_data_mem_write_data
    data_mem_address = alu_core_result;
    data_mem_write_data = regs_rs2_data;
  end

  always_comb begin : tock_mux_next_pc_select
    mux_next_pc_select_sel = next_pc_select;
    mux_next_pc_select_in0 = adder_pc_plus_4_result;
    mux_next_pc_select_in1 = adder_pc_plus_immediate_result;
    mux_next_pc_select_in2 = {alu_core_result[31:1], 1'b0};
    mux_next_pc_select_in3 = 32'b0;
    /*mux_next_pc_select.tock();*/
  end

  always_comb begin : tock_program_counter
    program_counter_reset = reset;
    program_counter_write_enable = pc_write_enable;
    program_counter_next = mux_next_pc_select_out;
    /*program_counter.tock();*/
  end

  always_comb begin : tock_mux_reg_writeback
    mux_reg_writeback_sel = reg_writeback_select;
    mux_reg_writeback_in0 = alu_core_result;
    mux_reg_writeback_in1 = data_mem_read_data;
    mux_reg_writeback_in2 = adder_pc_plus_4_result;
    mux_reg_writeback_in3 = igen_immediate;
    mux_reg_writeback_in4 = 32'b0;
    mux_reg_writeback_in5 = 32'b0;
    mux_reg_writeback_in6 = 32'b0;
    mux_reg_writeback_in7 = 32'b0;
    /*mux_reg_writeback.tock();*/
  end

  always_comb begin : tock_reg_writeback
    regs_write_enable = regfile_write_enable;
    regs_rd_data = mux_reg_writeback_out;
    /*regs.tock();*/
  end

  //----------------------------------------

 /*private:*/
  adder #(
    // Template Parameters
    .WIDTH(32)
  ) adder_pc_plus_4(
  );
  adder #(
    // Template Parameters
    .WIDTH(32)
  ) adder_pc_plus_immediate(
  );
  alu alu_core(
  );
  multiplexer4 #(
    // Template Parameters
    .WIDTH(32)
  ) mux_next_pc_select(
  );
  multiplexer2 #(
    // Template Parameters
    .WIDTH(32)
  ) mux_operand_a(
  );
  multiplexer2 #(
    // Template Parameters
    .WIDTH(32)
  ) mux_operand_b(
  );
  multiplexer8 #(
    // Template Parameters
    .WIDTH(32)
  ) mux_reg_writeback(
  );
  single_register #(
    // Template Parameters
    .WIDTH(32),
    .INITIAL(rv_config::INITIAL_PC)
  ) program_counter(
  );
  regfile regs(
  );
  instruction_decoder idec(
  );
  immediate_generator igen(
  );
endmodule

`endif // SINGLECYCLE_DATAPATH_H
