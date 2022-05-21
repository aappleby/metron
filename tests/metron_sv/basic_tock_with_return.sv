`include "metron_tools.sv"

// Tock methods can return values.

module Module
(
  output int my_sig,
  // tock()
  output int tock_ret
);
/*public:*/


  always_comb begin : tock
    my_sig = 7;
    tock_ret = my_sig;
  end
endmodule
