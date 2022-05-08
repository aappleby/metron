`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
(
  input logic clock,
  output logic[7:0] sub_reg
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction


/*private:*/

  function void tick();
    sub_reg <= sub_reg + 1;
  endfunction
  //----------------------------------------
  always_comb begin
    tock();
  end


  always_ff @(posedge clock) begin
    tick();
  end

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

  function void tock();
    /*submod.tock*/;
  endfunction

  Submod submod(
    .clock(clock),
    .sub_reg(submod_sub_reg)
  );
  logic[7:0] submod_sub_reg;

  //----------------------------------------
  always_comb begin
    tock_get_submod_reg_ret = tock_get_submod_reg();
    tock();
  end
endmodule
