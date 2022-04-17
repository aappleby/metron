`include "metron_tools.sv"

// Intentionally-failing test that should trigger the lockstep test timeout

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
    done = counter >= 32'hFFFF0000;
  end

  always_comb begin /*result*/
    result = counter;
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

