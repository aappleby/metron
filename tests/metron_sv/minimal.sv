`include "metron_tools.sv"

// Empty module should pass.

module Module
(
input logic clock
);

always_comb begin
end


always_ff @(posedge clock) begin
end

endmodule
