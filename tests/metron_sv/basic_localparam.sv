`include "metron_tools.sv"

// Static const members become SV localparams

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin : tock
  end

/*private:*/

  localparam int my_val = 7;

  task automatic tick();
    my_reg <= my_reg + my_val;
  endtask
  always_ff @(posedge clock) tick();

  logic[6:0] my_reg;
endmodule
