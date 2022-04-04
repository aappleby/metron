`include "metron_tools.sv"

// Public register member variables get moved to the output port list.

module Module
(
  input logic clock,
  output logic my_reg
);
/*public:*/

  task tick(); 
    my_reg <= 1;
  endtask
  always_ff @(posedge clock) tick();

  /*logic<1> my_reg;*/
endmodule

