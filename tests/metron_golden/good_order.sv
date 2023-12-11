`include "metron/metron_tools.sv"

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    my_sig = my_reg_;
    /*tick();*/
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg_ <= 1;
  end

  logic my_sig;
  logic my_reg_;
endmodule
