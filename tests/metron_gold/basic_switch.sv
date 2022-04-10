`include "metron_tools.sv"

// Simple switch statements are OK.

module Module
(
  input logic clock,
  input logic[1:0] selector
);
/*public:*/

  task tick(); 
    case(selector) 
      /*case*/ 0: my_reg <= 17; /*break;*/
      /*case*/ 1: my_reg <= 22; /*break;*/
      /*case*/ 2: my_reg <= 30; /*break;*/
      /*case*/ 3, // fallthrough
      /*case*/ 4,
      /*case*/ 5,
      /*case*/ 6: my_reg <= 72; /*break;*/
    endcase
  endtask
  always_ff @(posedge clock) tick();

/*private:*/
  logic[7:0] my_reg;
endmodule

