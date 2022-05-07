`include "metron_tools.sv"

// Static const members become SV localparams

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    /*tick()*/;
  endfunction

/*private:*/

  localparam int my_val = 7;

  function void tick();
    my_reg <= my_reg + my_val;
  endfunction

  logic[6:0] my_reg;

  //----------------------------------------
  always_comb begin
    tock();
  end


  always_ff @(posedge clock) begin
    tick();
  end

endmodule
