`include "metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module
(
  input logic clock,
  output logic my_reg
);
/*public:*/
  task automatic tock();
    /*tick()*/;
  endtask
  always_comb tock();

/*private:*/
  task automatic tick();
    my_reg <= my_reg + 1;
  endtask
  always_ff @(posedge clock) tick();
endmodule
