`include "metron_tools.sv"

// All the combos of tasks/funcs should work from ticks and tocks.

module Module
(
  input logic clock,
  output logic[7:0] my_sig
);
/*public:*/


  always_comb begin : tock
    public_task(public_func(17));
  end

  function public_task(logic[7:0] x);
    my_sig = x + 7;
  endfunction

  function logic[7:0] public_func(logic[7:0] x);
    public_func = my_reg1 + private_func(5);
  endfunction

/*private:*/

  task automatic tick();
    private_task(private_func(33));
    my_reg2 <= my_reg2 + 1;
  endtask
  always_ff @(posedge clock) tick();

  task automatic private_task(logic[7:0] x);
    my_reg1 <= my_reg1 + private_func(16);
  endtask
  logic[7:0] private_task_x;

  function logic[7:0] private_func(logic[7:0] y);
    private_func = my_reg1 + y;
  endfunction

  logic[7:0] my_reg1;
  logic[7:0] my_reg2;
endmodule
