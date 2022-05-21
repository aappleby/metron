`include "metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  // global clock
  input logic clock,
  // get_reg() ports
  output logic[6:0] get_reg_ret
);
/*public:*/

  function logic[6:0] get_reg();
    get_reg = my_reg;
  endfunction
  always_comb get_reg_ret = get_reg();

  always_comb begin : tock
  end


/*private:*/

  always_ff @(posedge clock) begin : tick
    my_reg <= my_reg + 1;
  end

  logic[6:0] my_reg;
endmodule
