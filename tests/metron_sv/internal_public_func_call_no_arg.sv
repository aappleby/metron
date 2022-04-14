`include "metron_tools.sv"

// We can call a function with no params from inside the module.

module Module
(
  input logic clock
);
/*public:*/

  function int foo();
    foo = 2;
  endfunction

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  logic[2:0] my_reg;

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= foo + 1;
  end

endmodule

