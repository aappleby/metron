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
    lfsr = 32'hDEADBEEF;
  end

  always_comb begin /*done*/
    done = counter >= 100;
  end

  always_comb begin /*result*/
    result = lfsr;
  end

  always_comb begin /*tock*/
    /*tick()*/;
  end

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    logic next_bit;

    counter <= counter + 1;
    next_bit = lfsr[31] ^ lfsr[21] ^ lfsr[1] ^ lfsr[0];
    lfsr <= {lfsr, next_bit};
  end

  logic[31:0] counter;
  logic[31:0] lfsr;

endmodule

