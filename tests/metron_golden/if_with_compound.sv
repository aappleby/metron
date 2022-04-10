// If statements whose sub-blocks contain submodule calls _must_ use {}.

module Submod
(
  input logic clock
);

  task tick();
  endtask
  always_ff @(posedge clock) tick();
endmodule


module Module
(
  input logic clock
);

  task tick();
    if (1) begin
      /*submod.tick()*/;
    end
  endtask
  always_ff @(posedge clock) tick();

  Submod submod(
    // Inputs
    .clock(clock)
    // Outputs
  );

endmodule

