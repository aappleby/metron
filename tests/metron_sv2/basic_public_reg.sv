`include "metron/tools/metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module (
  // global clock
  input logic clock,
  // output registers
  logic my_reg
);
/*public:*/
  always_comb begin : tock
    /*tick();*/
  end

/*private:*/
  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + 1;
  end
endmodule