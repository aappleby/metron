// This file should always pass in everything

module Module (input logic clock, output logic[7:0] out);

  initial begin
    out = 0;
  end

  always_ff @ (posedge clock) begin
    out = out + 1;
  end

endmodule
