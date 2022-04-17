`include "metron_tools.sv"

module Module
(
  input logic clock,
  output logic done,
  output logic[31:0] result
);
/*public:*/
  initial begin /*Module*/
    counter = 0;
  end

  always_comb begin /*done*/
    done = counter >= 7;
  end

  always_comb begin /*result*/
    logic[31:0] c;

    c = counter;
    result = c;
  end

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    counter <= counter + 1;
  end

  logic[31:0] counter;

endmodule

