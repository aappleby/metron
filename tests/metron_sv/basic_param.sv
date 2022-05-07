`include "metron_tools.sv"

// Template parameters become SV module parameters.

module Module
#(parameter int SOME_CONSTANT = 7)
(
  input logic clock
);
/*public:*/

  function void tock();
    tick();
  endfunction

/*private:*/

  function void tick();
    my_reg = SOME_CONSTANT;
  endfunction

  logic[6:0] my_reg;

  always_comb begin
    tock();
  end

endmodule
