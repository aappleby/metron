`include "metron/tools/metron_tools.sv"

// Declaration order _matters_ - a tock() that reads a reg before the tick()
// that writes it is OK.

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    my_sig = my_reg;
    /*tick();*/
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= 1;
  end

  logic my_sig;
  logic my_reg;
endmodule
