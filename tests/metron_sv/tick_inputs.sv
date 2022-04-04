`include "metron_tools.sv"

// Tick inputs should become input ports.

module Module
(
  input logic clock,
  input logic[6:0] my_input,
  output logic[6:0] my_reg
);
/*public:*/

  task tick(); 
    my_reg <= my_reg + my_input;
  endtask
  always_ff @(posedge clock) tick();

  /*logic<7> my_reg;*/
endmodule

