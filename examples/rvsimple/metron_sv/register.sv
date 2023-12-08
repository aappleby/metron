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
);
  parameter WIDTH = 32;
  parameter INITIAL = 0;
 /*public:*/

  initial begin value = INITIAL; end

  always_comb begin : tock  /*tick();*/ end

 /*private:*/
  always_comb begin : tick
    if (reset)
      value = INITIAL;
    else if (write_enable)
      value = next;
  end
endmodule

`endif // REGISTER_H
