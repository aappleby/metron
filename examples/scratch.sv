`include "metron/tools/metron_tools.sv"

// If a module calls a submod's functions in the "wrong" order, we should catch
// it.

`include "metron/tools/metron_tools.sv"

module Submod (
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tick1  y <= y + z; end
  always_ff @(posedge clock) begin : tick2  z <= z + 1; end

/*private:*/
  int y;
  int z;
endmodule

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    /*x.tick1();*/
    /*x.tick2();*/
  end

/*private:*/

  Submod x(
    // Global clock
    .clock(clock)
  );
endmodule
