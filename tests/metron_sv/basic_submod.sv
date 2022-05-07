`include "metron_tools.sv"

// Modules can contain other modules.

module Submod
(
  input logic clock
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction

/*private:*/

  function void tick();
    sub_reg <= sub_reg + 1;
  endfunction

  logic[7:0] sub_reg;

  always_comb begin
    tock();
  end


  always_ff @(posedge clock) begin
    tick();
  end

endmodule

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    submod_tock;
  endfunction

  Submod submod(
    // Inputs
    .clock(clock)
    // Outputs
  );


  always_comb begin
    tock();
  end

endmodule
