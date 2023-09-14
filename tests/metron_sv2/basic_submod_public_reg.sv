`include "metron/tools/metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod (
  // global clock
  input logic clock,
  // output registers
  logic[7:0] sub_reg
);
/*public:*/

  always_comb begin : tock
    /*tick()*/;
  end


/*private:*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + 1;
  end

endmodule;

module Module (
  // global clock
  input logic clock,
  // get_submod_reg() ports
  output logic[7:0] get_submod_reg_ret
);
/*public:*/

  always_comb begin : get_submod_reg /*const*/
    get_submod_reg_ret = submod_sub_reg;
  end

  always_comb begin : tock
    /*submod.tock()*/;
  end

endmodule;