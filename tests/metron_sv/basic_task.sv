`include "metron_tools.sv"

// Private non-const methods should turn into SV tasks.

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    tick();
  endfunction

/*private:*/

  function void tick();
    /*some_task()*/;
  endfunction

  function void some_task();
    my_reg <= my_reg + my_reg2 + 3;
    some_task2();
  endfunction

  function void some_task2();
    my_reg2 <= my_reg2 + 3;
  endfunction

  logic[7:0] my_reg;
  logic[7:0] my_reg2;

  //----------------------------------------
  always_comb begin
    tock();
  end


  always_ff @(posedge clock) begin
    some_task();
  end

endmodule
