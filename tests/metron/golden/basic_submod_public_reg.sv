`include "metron/metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
(
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] sub_reg_
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick();
  end


/*private:*/

  task automatic tick();
    sub_reg_ <= sub_reg_ + 1;
  endtask

endmodule

module Module
(
  // global clock
  input logic clock,
  // get_submodreg() ports
  output logic[7:0] get_submodreg_ret
);
/*public:*/

  always_comb begin : get_submodreg
    get_submodreg_ret = submod_sub_reg_;
  end

  always_comb begin : tock
    /*submod.tock();*/
  end

  Submod submod(
    // Global clock
    .clock(clock),
    // Output registers
    .sub_reg_(submod_sub_reg_)
  );
  logic[7:0] submod_sub_reg_;
endmodule
