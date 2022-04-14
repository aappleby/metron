`include "metron_tools.sv"

// Public functions should turn into always_comb and their args should turn
// into input ports.

module Module
(
  input logic clock,
  output logic my_reg
);
/*public:*/

  function logic[7:0] func1();
    func1 = 23;
  endfunction

  function logic[7:0] func2(logic[7:0] func2_arg);
    func2 = 17 + func2_arg;
  endfunction

  always_ff @(posedge clock) begin /*tick*/
    my_reg <= 1;
  end

endmodule

