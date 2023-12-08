`include "metron/metron_tools.sv"

// Template parameters become SV module parameters.

module Module (
  // global clock
  input logic clock
);
  parameter SOME_CONSTANT = 7;
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick();
  end

/*private:*/

  task automatic tick();
    my_reg_ <= my_reg_ + SOME_CONSTANT;
  endtask

  logic[6:0] my_reg_;
endmodule
