// RISC-V SiMPLE SV -- Single-cycle RISC-V core
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RISCV_CORE_H
`define RISCV_CORE_H

`include "config.sv"
`include "constants.sv"
`include "data_memory_interface.sv"
`include "metron/metron_tools.sv"
`include "singlecycle_ctlpath.sv"
`include "singlecycle_datapath.sv"

module riscv_core (
);
 /*public:*/


  always_comb begin : tock_pc
    /*datapath.tock_pc();*/
    pc = datapath_pc;
  end

  always_comb begin : tock_execute
    datapath_inst = inst;
    /*datapath.tock_instruction_decoder();*/
    /*datapath.tock_immediate_generator();*/

    ctlpath_inst_opcode = datapath_inst_opcode;
    ctlpath_inst_funct3 = datapath_inst_funct3;
    ctlpath_inst_funct7 = datapath_inst_funct7;

    /*ctlpath.tock_alu_function();*/
    /*ctlpath.tock_alu_operand_select();*/

    datapath_alu_function = ctlpath_alu_function;
    datapath_alu_operand_a_select = ctlpath_alu_operand_a_select;
    datapath_alu_operand_b_select = ctlpath_alu_operand_b_select;

    /*datapath.tock_reg_read();*/
    /*datapath.tock_mux_operand_a();*/
    /*datapath.tock_mux_operand_b();*/
    /*datapath.tock_alu();*/

    /*ctlpath.tock_data_mem_enable();*/
    /*datapath.tock_data_mem_write_data();*/
    dmem_read_enable = ctlpath_data_mem_read_enable;
    dmem_write_enable = ctlpath_data_mem_write_enable;
    dmem_data_format = datapath_inst_funct3;
    dmem_address = datapath_data_mem_address;
    dmem_write_data = datapath_data_mem_write_data;
    /*dmem.tock_bus();*/

    ctlpath_alu_result_equal_zero = datapath_alu_result_equal_zero;
    /*ctlpath.tock_pc_write_enable();*/
    /*ctlpath.tock_regfile_write_enable();*/
    /*ctlpath.tock_reg_writeback_select();*/
    /*ctlpath.tock_next_pc_select();*/

    //----------

    bus_address = dmem_bus_address;
    bus_write_data = dmem_bus_write_data;
    bus_byte_enable = dmem_bus_byte_enable;
    bus_read_enable = dmem_bus_read_enable;
    bus_write_enable = dmem_bus_write_enable;
  end

  always_comb begin : tock_writeback
    dmem_bus_read_data = bus_read_data;
    /*dmem.tock_read_data();*/

    datapath_next_pc_select = ctlpath_next_pc_select;
    /*datapath.tock_adder_pc_plus_4();*/
    /*datapath.tock_adder_pc_plus_immediate();*/
    /*datapath.tock_mux_next_pc_select();*/

    datapath_reset = reset;
    datapath_pc_write_enable = ctlpath_pc_write_enable;
    /*datapath.tock_program_counter();*/

    datapath_reg_writeback_select = ctlpath_reg_writeback_select;
    datapath_data_mem_read_data = dmem_read_data;
    /*datapath.tock_mux_reg_writeback();*/

    datapath_regfile_write_enable = ctlpath_regfile_write_enable;
    /*datapath.tock_reg_writeback();*/
  end

  //----------------------------------------

 /*private:*/
  singlecycle_datapath datapath(
  );
  singlecycle_ctlpath ctlpath(
  );
  data_memory_interface dmem(
  );
endmodule

`endif // RISCV_CORE_H
