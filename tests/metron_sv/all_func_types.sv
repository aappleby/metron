`include "metron_tools.sv"

module Module
(
  input logic clock,
  output int my_sig1,
  output int my_sig2,
  output int my_sig3,
  output int my_sig4,
  output int my_sig5,
  output int my_sig6a,
  output int my_sig6b,
  output int my_reg1,
  output int my_reg2,
  output int func_no_params_return_ret,
  input int func_params_return_x,
  output int func_params_return_ret,
  output int tock_no_params_return_ret,
  input int tock_params_no_return_x,
  input int tock_params_return_x,
  output int tock_params_return_ret,
  input int tock_calls_funcs_x,
  input int tock_calls_tock_x,
  output int tock_calls_tock_ret,
  input int tick_params_x
);
/*public:*/


  /*
  // but why would you do this?
  void func_no_params_no_return() {
  }

  // or this?
  void func_params_no_return(int x) {
  }
  */

  function int func_no_params_return();
    func_no_params_return = 1;
  endfunction
  always_comb func_no_params_return_ret = func_no_params_return();

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

  always_comb begin : tock_calls_funcs
    my_sig5 = 12 + my_func5(tock_calls_funcs_x);
  end

  function int my_func5(int x);  my_func5 = x + 1; endfunction

  always_comb begin : tock_calls_tock
    my_sig6a = 12;
    tock_called_by_tock_x = my_sig6a;
    tock_calls_tock_ret = 0;
  end

  always_comb begin : tock_called_by_tock
    my_sig6b = tock_called_by_tock_x;
  end
  int tock_called_by_tock_x;

  //----------


  always_ff @(posedge clock) begin : tick_no_params
    my_reg1 <= my_reg1 + 1;
  end

  always_ff @(posedge clock) begin : tick_params
    my_reg2 <= my_reg2 + tick_params_x;
  end
endmodule
