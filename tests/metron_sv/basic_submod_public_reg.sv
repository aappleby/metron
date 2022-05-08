`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
(
  input logic clock,
  output logic[7:0] sub_reg
);
/*public:*/

  task  tock();
    /*tick()*/;
  endtask
  always_comb tock();


/*private:*/

  task  tick();
    sub_reg <= sub_reg + 1;
  endtask
  always_ff @(posedge clock) tick();

endmodule

module Module
(
  input logic clock,
  output logic[7:0] tock_get_submod_reg_ret
);
/*public:*/

  function logic[7:0] tock_get_submod_reg();
    tock_get_submod_reg = submod_sub_reg;
  endfunction
  always_comb tock_get_submod_reg_ret = tock_get_submod_reg();

  task  tock();
    /*submod.tock*/;
  endtask
  always_comb tock();

  Submod submod(
    .clock(clock),
    .sub_reg(submod_sub_reg)
  );
  logic[7:0] submod_sub_reg;

endmodule
