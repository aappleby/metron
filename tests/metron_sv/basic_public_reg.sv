`include "metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    tick();
  endfunction

/*private:*/

  function void tick();
    my_reg = 1;
  endfunction

  logic my_reg;

  always_comb begin
    tock();
  end

endmodule
