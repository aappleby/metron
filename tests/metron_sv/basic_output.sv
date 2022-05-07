`include "metron_tools.sv"

// Getter methods should turn into outputs.

module Module
(
  input logic clock,
  output logic[6:0] tock_get_reg_ret
);
/*public:*/

  function logic[6:0] tock_get_reg();
    tock_get_reg = my_reg;
  endfunction

  function void tock();
    /*tick()*/;
  endfunction


/*private:*/

  function void tick();
    my_reg <= my_reg + 1;
  endfunction

  logic[6:0] my_reg;

  always_comb begin
    tock_get_reg_ret = tock_get_reg();
    tock();
  end


  always_ff @(posedge clock) begin
    tick();
  end

endmodule
