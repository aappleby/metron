`include "metron/metron_tools.sv"

// Simple switch statements are OK.

module Module (
  // global clock
  input logic clock,
  // tock() ports
  input logic[1:0] tock_selector
);
/*public:*/

  always_comb begin : tock
    tick_selector = tock_selector;
    /*tick(selector);*/
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    case(tick_selector)
      0: // comment
         begin my_reg <= 17; /*break;*/ end
      1:  // comment
         begin my_reg <= 22; /*break;*/ end
      2:  begin my_reg <= 30; /*break;*/ end
      3, // fallthrough
      4,
      5,
      6:  begin my_reg <= 72; /*break;*/ end
    endcase
  end
  logic[1:0] tick_selector;

  logic[7:0] my_reg;
endmodule
