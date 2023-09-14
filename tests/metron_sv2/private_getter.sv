`include "metron/tools/metron_tools.sv"

// Private getter methods are OK

module Module (
  // tock() ports
  output int tock_ret
);
/*public:*/


  always_comb begin : tock
    my_sig = my_getter();
    tock_ret = my_sig;
  end

/*private:*/

  function int my_getter(); /*const*/
    my_getter = 12;
  endfunction

endmodule
