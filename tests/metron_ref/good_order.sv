`include "metron_tools.sv"

// Declaration order _matters_ - a tock() that reads a reg before the tick()
// that writes it is OK.

module Module
(
  input logic clock,
  output logic my_sig,
  output logic my_reg
);
/*public:*/

  always_comb begin : tock
    my_sig = my_reg;
  end

  task tick(); 
    my_reg <= 1;
  endtask
  always_ff @(posedge clock) tick();

  /*logic<1> my_sig;*/
  /*logic<1> my_reg;*/
endmodule

