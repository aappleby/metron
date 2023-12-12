`include "metron/metron_tools.sv"

// Case statements are allowed to have fallthrough, though the SV syntax is
// a bit different.

module Module (
  // global clock
  input logic clock,
  // output signals
  output logic[7:0] tock_result
);
/*public:*/

  always_comb begin : tock
    case(my_reg_)
      0, // can we stick comments in here?
      1,
      2:
        tock_result = 10;
        /*break;*/
      3: begin
        tock_result = 20;
        /*break;*/
      end
      default:
        tock_result = 30;
        /*break;*/
    endcase

    tick_inc = tock_result;
    /*tick(tock_result);*/
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg_ <= my_reg_ + tick_inc;
  end
  logic[7:0] tick_inc;

  logic[7:0] my_reg_;
endmodule
