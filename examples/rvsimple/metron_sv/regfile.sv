// RISC-V SiMPLE SV -- register file
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef REGFILE_H
`define REGFILE_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module regfile
(
  // global clock
  input logic clock,
  // input signals
  input logic write_enable,
  input logic[4:0] rd_address,
  input logic[4:0] rs1_address,
  input logic[4:0] rs2_address,
  input logic[31:0] rd_data,
  // output signals
  output logic[31:0] rs1_data,
  output logic[31:0] rs2_data
);
 /*public:*/

 /*private:*/
  // 32 registers of 32-bit width
  logic[31:0] register_[32];

 /*public:*/
  // Read ports for rs1 and rs1
  always_comb begin : tock1
    rs1_data = register_[rs1_address];
    rs2_data = register_[rs2_address];
  end

  // Register x0 is always 0
  initial begin register_[0] = 32'b0; end

  always_ff @(posedge clock) begin : tock  tick(); end

 /*private:*/
  // Write port for rd
  task automatic tick();
    if (write_enable)
      if (rd_address != 5'b0) register_[rd_address] <= rd_data;
  endtask
endmodule

`endif // REGFILE_H
