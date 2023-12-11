// RISC-V SiMPLE SV -- generic register
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef REGISTER_H
`define REGISTER_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module single_register (
  // global clock
  input logic clock,
  // input signals
  input logic reset,
  input logic write_enable,
  input logic[WIDTH-1:0] next,
  // output registers
  output logic[WIDTH-1:0] value_
);
  parameter WIDTH = 32;
  parameter INITIAL = 0;
 /*public:*/

  initial begin value_ = INITIAL; end

  always_ff @(posedge clock) begin : tock  tick(); end

 /*private:*/
  task automatic tick();
    if (reset)
      value_ <= INITIAL;
    else if (write_enable)
      value_ <= next;
  endtask
endmodule

`endif // REGISTER_H
