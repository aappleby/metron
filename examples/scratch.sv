`include "metron/metron_tools.sv"

module Module (
  // output signals
  output int my_sig6b,
  // tock_calls_tock() ports
  input int tock_calls_tock_x,
  output int tock_calls_tock_ret
);
/*public:*/

  always_comb begin : tock_calls_tock
    tock_called_by_tock_x = tock_calls_tock_x;
    /*tock_called_by_tock(x);*/
    tock_calls_tock_ret = 0;
  end

/*private:*/
  always_comb begin : tock_called_by_tock
    my_sig6b = tock_called_by_tock_x;
  end
  int tock_called_by_tock_x;
endmodule
