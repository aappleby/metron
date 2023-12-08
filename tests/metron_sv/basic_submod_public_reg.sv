`include "metron/metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod (
  // global clock
  input logic clock
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

module Module (
  // global clock
  input logic clock,
  // get_submod_reg() ports
  output logic[7:0] get_submod_reg_ret
);
/*public:*/

  always_comb begin : get_submod_reg
    get_submod_reg_ret = submod__sub_reg_;
  end

  always_comb begin : tock
    /*submod_.tock();*/
  end

  Submod submod_(
    // Global clock
    .clock(clock)
  );
endmodule
