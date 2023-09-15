`include "metron/tools/metron_tools.sv"

// Zero-initializing structs should work for convenience.

module Module (
  // output signals
  output int my_struct1
);
/*public:*/

  always_comb begin : tock
    my_struct1 = func(2);
  end

/*private:*/

  function int func(int x);
    func = x + 1;
  endfunction

endmodule
