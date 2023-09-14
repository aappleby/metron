`include "metron/tools/metron_tools.sv"

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
/*public:*/


  always_comb begin : tock
    public_task_x = public_func(tock_z);
    dummy=/*public_task(public_func(z))*/
    public_task_x = public_func(z);
    tick_w = tock_z;
    /*tick(z);*/
  end

  always_comb begin : public_task
    my_sig = public_task_x + 7;
    public_task_ret = 0;
  end

  function logic[7:0] public_func(x);
    public_func_ret = my_reg1 + private_func(x);
  endfunction

/*private:*/

  always_ff @(posedge clock) begin : tick
    private_task_x = private_func(tick_w);
    /*private_task(private_func(w));*/
    my_reg2 <= my_reg2 + 1;
  end

  task automatic private_task(x);
    my_reg1 <= my_reg1 + private_func(x);
  endtask

  function logic[7:0] private_func(y);
    private_func_ret = my_reg1 + y;
  endfunction

  logic[7:0] my_reg1;
  logic[7:0] my_reg2;
endmodule
