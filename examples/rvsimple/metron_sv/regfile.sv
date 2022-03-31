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
  initial begin : init
    regs[0] = 32'd0;
  end

  always_comb begin
    rs1_data = regs[rs1_address];
  end

  always_comb begin
    rs2_data = regs[rs2_address];
  end

  // Write port for rd
  task tick(); 
    if (write_enable && rd_address != 5'd0) begin
      regs[rd_address] = rd_data;
    end
  endtask
  always_ff @(posedge clock) tick();

 /*private:*/
  // 32 registers of 32-bit width
  logic[31:0] regs[32];
endmodule;

`endif  // RVSIMPLE_REGFILE_H

