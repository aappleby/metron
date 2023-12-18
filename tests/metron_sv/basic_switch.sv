`include "metron/metron_tools.sv"

// Simple switch statements are OK.

module Module
(
  // global clock
  input logic clock,
  // tock() ports
  input logic[1:0] tock_selector
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick(tock_selector);
  end

/*private:*/

  task automatic tick(logic[1:0] selector);
    case(selector)
      0: // comment
        my_reg_ <= 17; /*break;*/
      1:  // comment
        my_reg_ <= 22; /*break;*/
      2: my_reg_ <= 30; /*break;*/
      3, // fallthrough
      4,
      5,
      6: my_reg_ <= 72; /*break;*/
    endcase
  endtask

  logic[7:0] my_reg_;
endmodule
