`include "metron_tools.sv"

// Simple switch statements are OK.

module Module
(
  input logic clock,
  input logic[1:0] tock_selector
);
/*public:*/

  function void tock(logic[1:0] selector);
    tick_selector = selector;
    /*tick(selector)*/;
  endfunction

/*private:*/

  function void tick(logic[1:0] selector);
    case(selector)
      0: // comment
        my_reg <= 17;
      1:  // comment
        my_reg <= 22;
      2: my_reg <= 30;
      3, // fallthrough
      4,
      5,
      6: my_reg <= 72;
    endcase
  endfunction

  logic[7:0] my_reg;
  //----------------------------------------
  always_comb begin
    tock(tock_selector);
  end

  logic[1:0] tick_selector;

  always_ff @(posedge clock) begin
    tick(tick_selector);
  end
endmodule
