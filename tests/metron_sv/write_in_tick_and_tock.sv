`include "metron_tools.sv"

// Fields can be written in tick() or tock() but not both.

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    field = 0;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    field = 1;
  end

  logic field;
endmodule

