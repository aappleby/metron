`include "metron_tools.sv"

module Module
(
  input logic clock,
  output int my_sig1,
  output int my_sig2,
  output int my_sig3,
  output int my_sig4,
  output int my_reg1,
  output int my_reg2,
  output int func_no_params_return_ret,
  input int func_params_return_x,
  output int func_params_return_ret,
  output int tock_no_params_return_ret,
  input int tock_params_no_return_x,
  input int tock_params_return_x,
  output int tock_params_return_ret,
  input int tick_params_x
);
/*public:*/


  /*
  void func_no_params_no_return() {
  }
  */

  function int func_no_params_return();
    func_no_params_return = 1;
  endfunction
  always_comb func_no_params_return_ret = func_no_params_return();

  /*
  void func_params_no_return(int x) {
  }
  */

  function int func_params_return(int x);
    func_params_return = x + 1;
  endfunction
  always_comb func_params_return_ret = func_params_return(func_params_return_x);

  always_comb begin : tock_no_params_no_return
    int x;
    my_sig1 = 12;
    x = my_sig1;
  end

  always_comb begin : tock_no_params_return
    my_sig2 = 12;
    tock_no_params_return_ret = my_sig2;
  end

  always_comb begin : tock_params_no_return
    int y;
    my_sig3 = 12 + tock_params_no_return_x;
    y = my_sig3;
  end

  always_comb begin : tock_params_return
    my_sig4 = 12 + tock_params_return_x;
    tock_params_return_ret = my_sig4;
  end

  task automatic tick_no_params();
    my_reg1 <= my_reg1 + 1;
  endtask
  always_ff @(posedge clock) tick_no_params();

  task automatic tick_params(int x);
    my_reg2 <= my_reg2 + x;
  endtask
  always_ff @(posedge clock) tick_params(tick_params_x);
endmodule
