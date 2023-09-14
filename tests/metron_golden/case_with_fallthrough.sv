`include "metron/tools/metron_tools.sv"

// Case statements are allowed to have fallthrough, though the SV syntax is
// a bit different.

module Module (
  // global clock
  input logic clock,
  // tock() ports
  output logic[7:0] tock_ret
);
/*public:*/

  always_comb begin : tock
    logic[7:0] result;
    case(my_reg)
      0, // can we stick comments in here?
      1,
      2:
        result = 10;
      3: begin
        result = 20;
      end
      default:
        result = 30;
    endcase

    /*tick();*/
    tock_ret = result;
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + 1;
  end

  logic[7:0] my_reg;
endmodule
