`include "metron_tools.sv"

// Public functions should turn into always_comb and their args should turn
// into input ports.

module Module
(
  input logic clock,
  output logic my_reg,
  input logic[7:0] func2_arg,
  output logic[7:0] func1,
  output logic[7:0] func2
);
/*public:*/

  always_comb begin
    func1 = 23;
  end

  always_comb begin
    func2 = 17 + func2_arg;
  end

  always_ff @(posedge clock) begin : tick
    my_reg <= 1;
  end

endmodule

