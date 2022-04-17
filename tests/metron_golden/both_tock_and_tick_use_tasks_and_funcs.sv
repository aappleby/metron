`include "metron_tools.sv"

// All the combos of tasks/funcs should work from ticks and tocks.

module Module
(
  input logic clock,
  output logic[7:0] my_sig
);
/*public:*/


  always_comb begin /*tock*/
    public_task(public_func(17));
    /*tick()*/;
  end

  task public_task(logic[7:0] public_task_x);
    my_sig = public_task_x + 7;
  endtask

  function logic[7:0] public_func(logic[7:0] public_func_x);
    public_func = my_reg + private_func(5);
  endfunction

/*private:*/

  always_ff @(posedge clock) begin /*tick*/
    private_task(private_func(33));
  end

  task private_task(logic[7:0] private_task_x);
    my_reg = my_reg + private_func(16);
  endtask

  function logic[7:0] private_func(logic[7:0] private_func_y);
    private_func = my_reg + private_func_y;
  endfunction

  logic[7:0] my_reg;
endmodule

