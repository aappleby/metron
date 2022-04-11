// If statements whose sub-blocks contain submodule calls _must_ use {}.

module Submod
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tick
  end
endmodule


module Module
(
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    if (1) begin
      /*submod.tick()*/;
    end
  end

  Submod submod(
    // Inputs
    .clock(clock)
    // Outputs
  );

endmodule

