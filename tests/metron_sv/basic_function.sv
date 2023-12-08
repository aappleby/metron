`include "metron/metron_tools.sv"

// Private const methods should turn into SV functions.

module Module (
  // global clock
  input logic clock
);
/*public:*/

  always_ff @(posedge clock) begin : tock
    tick();
  end

/*private:*/

  task automatic tick();
    my_reg_ <= my_reg_ + some_func();
  endtask

  function logic[7:0] some_func() /*const*/;
    some_func = 3;
  endfunction

  logic[7:0] my_reg_;
endmodule
