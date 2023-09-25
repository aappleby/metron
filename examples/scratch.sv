`include "metron/tools/metron_tools.sv"

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    int bar;
    bar = 5;
    foo <= foo + 1;
    bar = bar + 1;
  end

/*private:*/
  int foo;
endmodule
