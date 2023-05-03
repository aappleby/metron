`include "metron/tools/metron_tools.sv"

// Getter methods should turn into outputs.

module Module (
  // global clock
  input logic clock,
  // get_reg() ports
  output logic[6:0] get_reg_ret
);
/*public*/

  always_comb begin : get_reg
    get_reg_ret = my_reg;
  end

  always_comb begin : tock
  end


/*private*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + 1;
  end

  logic[6:0] my_reg;
endmodule
