`include "metron/metron_tools.sv"

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
    public_task(public_func(tock_z));
    tick_w = tock_z;
    /*tick(z);*/
  end

  task automatic public_task(logic[7:0] x);
    my_sig = x + 7;
  endtask

  function logic[7:0] public_func(logic[7:0] x);
    public_func = my_reg1_ + private_func(x);
  endfunction

/*private:*/

  always_ff @(posedge clock) begin : tick
    private_task(private_func(tick_w));
    my_reg2_ <= my_reg2_ + 1;
  end
  int tick_w;

  task automatic private_task(logic[7:0] x);
    my_reg1_ <= my_reg1_ + private_func(x);
  endtask

  function logic[7:0] private_func(logic[7:0] y);
    private_func = my_reg1_ + y;
  endfunction

  logic[7:0] my_reg1_;
  logic[7:0] my_reg2_;
endmodule
