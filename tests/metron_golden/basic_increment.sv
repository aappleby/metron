`include "metron_tools.sv"

// Increment/decrement should be translated into equivalent Verilog, but they
// do _not_ return the old/new value.

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction

/*private:*/

  function void tick();
    my_reg1 = my_reg1 + 1;
    my_reg2 = my_reg2 + 1;
    my_reg3 = my_reg3 - 1;
    my_reg4 = my_reg4 - 1;
  endfunction

  int my_reg1;
  int my_reg2;
  int my_reg3;
  int my_reg4;

  //----------------------------------------
  always_comb begin
    tock();
  end


  always_ff @(posedge clock) begin
    tick();
  end

endmodule
