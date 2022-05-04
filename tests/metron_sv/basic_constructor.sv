`include "metron_tools.sv"

// Modules can use constructors to initialize state

module Module
(
  input logic clock,
  output logic[7:0] tock_ret
);
/*public:*/

  initial begin /*Module*/
    my_reg = 7;
  end

  function logic[7:0] tock();
    tock = my_reg;
  endfunction
  always_comb tock_ret = tock();

/*private:*/

  logic[7:0] my_reg;
endmodule


`ifdef IVERILOG
module Test;
  logic clock;
  logic[7:0] tock;
  Module mod(.clock(clock), .tock(tock));
  initial begin
    if (tock != 7) $display("FAIL");
  end
endmodule
`endif


