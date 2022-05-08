`include "metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module
(
  input logic clock,
  output logic my_reg
);
/*public:*/
  function void tock();
    /*tick()*/;
  endfunction

/*private:*/
  function void tick();
    my_reg <= my_reg + 1;
  endfunction

  //----------------------------------------

  always_comb tock();
  always_ff @(posedge clock) tick();

endmodule
