`include "metron_tools.sv"

// We can't call a function with a param from inside the module.

module Module
(
  input logic clock
);
/*public:*/

  function int foo(logic[7:0] foo_baz);
    foo = foo_baz + 2;
  endfunction

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  logic[2:0] my_reg;

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= foo(7) + 1;
  end

endmodule

