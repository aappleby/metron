`include "metron/metron_tools.sv"

// Multiple tick methods are OK as long as they're all called by tock()

module Module (
  // global clock
  input logic clock,
  // tock() ports
  output logic[7:0] tock_ret
);
/*public:*/

  always_comb begin : tock
    logic[7:0] result;
    result = my_reg1_ + my_reg2_;
    /*tick1();*/
    /*tick2();*/
    tock_ret = result;
  end

/*private:*/

  always_ff @(posedge clock) begin : tick1
    my_reg1_ <= 0;
  end

  always_ff @(posedge clock) begin : tick2
    my_reg2_ <= 1;
  end

  logic[7:0] my_reg1_;
  logic[7:0] my_reg2_;
endmodule
