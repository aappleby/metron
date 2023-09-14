`include "metron/tools/metron_tools.sv"

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
        my_reg <= 17; /*break*/;
      1:  // comment
        my_reg <= 22; /*break*/;
      2: my_reg <= 30; /*break*/;
      3, // fallthrough
      4,
      5,
      6: my_reg <= 72; /*break*/;
    endcase
  end

  logic[7:0] my_reg;
endmodule
