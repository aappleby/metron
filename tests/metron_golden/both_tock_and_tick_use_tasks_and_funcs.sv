`include "metron_tools.sv"

// All the combos of tasks/funcs should work from ticks and tocks.

// Yosys bug - fails to parse if we pass constants between the functions
// https://github.com/YosysHQ/yosys/issues/3327

module Module
(
  input logic clock,
  output logic[7:0] my_sig,
  input int tock_z
);
/*public:*/


  always_comb begin : tock
    logic[7:0] dummy;
    dummy = public_task(public_func(tock_z));
    tick_w = tock_z;
  end

  function logic[7:0] public_task(logic[7:0] x);
    my_sig = x + 7;
    public_task = 0;
  endfunction

  function logic[7:0] public_func(logic[7:0] x);
    public_func = my_reg1 + private_func(x);
  endfunction

/*private:*/

  task automatic tick(int w);
    private_task(private_func(w));
    my_reg2 <= my_reg2 + 1;
  endtask
  int tick_w;
  always_ff @(posedge clock) tick(tick_w);

  task automatic private_task(logic[7:0] x);
    my_reg1 <= my_reg1 + private_func(x);
  endtask

  function logic[7:0] private_func(logic[7:0] y);
    private_func = my_reg1 + y;
  endfunction

  logic[7:0] my_reg1;
  logic[7:0] my_reg2;
endmodule
