`include "metron_tools.sv"

// All the combos of tasks/funcs should work from ticks and tocks.

// Yosys bug - fails to parse if we pass constants between the functions
// https://github.com/YosysHQ/yosys/issues/3327

module Module (
  // global clock
  input logic clock,
  // output signals
  output logic[7:0] my_sig,
  // tock() ports
  input int tock_z
);
/*public*/


  always_comb begin : tock
    logic[7:0] dummy;
    public_task_x = public_func(tock_z);
    dummy = public_task_ret;
    tick_w = tock_z;
  end

  always_comb begin : public_task
    my_sig = public_task_x + 7;
    public_task_ret = 0;
  end
  logic[7:0] public_task_x;
  logic[7:0] public_task_ret;

  function logic[7:0] public_func(logic[7:0] x);
    public_func = my_reg1 + private_func(x);
  endfunction

/*private*/

  always_ff @(posedge clock) begin : tick
    private_task(private_func(tick_w));
    my_reg2 <= my_reg2 + 1;
  end
  int tick_w;

  task automatic private_task(logic[7:0] x);
    my_reg1 <= my_reg1 + private_func(x);
  endtask

  function logic[7:0] private_func(logic[7:0] y);
    private_func = my_reg1 + y;
  endfunction

  logic[7:0] my_reg1;
  logic[7:0] my_reg2;
endmodule
