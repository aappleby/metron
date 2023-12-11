`include "metron/metron_tools.sv"

// Simple switch statements are OK.

module Module (
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
         begin my_reg_ <= 17; /*break;*/ end
      1:  // comment
         begin my_reg_ <= 22; /*break;*/ end
      2:  begin my_reg_ <= 30; /*break;*/ end
      3, // fallthrough
      4,
      5,
      6:  begin my_reg_ <= 72; /*break;*/ end
    endcase
  endtask

  logic[7:0] my_reg_;
endmodule
