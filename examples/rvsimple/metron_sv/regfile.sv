// RISC-V SiMPLE SV -- register file
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef RVSIMPLE_REGFILE_H
`define RVSIMPLE_REGFILE_H

`include "config.sv"
`include "constants.sv"
`include "metron_tools.sv"

module regfile
(
  input logic clock,
  input logic[4:0] rs1_address,
  input logic[4:0] rs2_address,
  input logic write_enable,
  input logic[4:0] rd_address,
  input logic[31:0] rd_data,
  output logic[31:0] rs1_data,
  output logic[31:0] rs2_data
);
 /*public:*/
  // Register x0 is always 0
  initial begin /*regfile*/ regs[0] = 32'd0; end

  /*logic<5> rs1_address;*/
  /*logic<5> rs2_address;*/

  /*logic<32> rs1_data;*/
  /*logic<32> rs2_data;*/
  /*logic<1> write_enable;*/
  /*logic<5> rd_address;*/
  /*logic<32> rd_data;*/

  always_comb begin /*tock_rs1_data*/ rs1_data = regs[rs1_address]; end
  always_comb begin /*tock_rs2_data*/ rs2_data = regs[rs2_address]; end

  always_comb begin /*tock*/
    /*tick()*/;
  end

 /*private:*/
  task tick(); 
    // Write port for rd
    if (write_enable && rd_address != 5'd0) begin
      regs[rd_address] <= rd_data;
    end
  endtask
  always_ff @(posedge clock) tick();

  // 32 registers of 32-bit width
  logic[31:0] regs[32];
endmodule;

`endif  // RVSIMPLE_REGFILE_H

