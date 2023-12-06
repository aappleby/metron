`include "metron/metron_tools.sv"

module Module (
  // global clock
  input logic clock,
  // output signals
  output int my_sig1,
  output int my_sig2,
  output int my_sig3,
  output int my_sig4,
  output int my_sig5a,
  output int my_sig5b,
  output int my_sig6a,
  // output registers
  output int my_reg1,
  output int my_reg2,
  // func_no_params_return() ports
  output int func_no_params_return_ret,
  // func_params_return() ports
  input int func_params_return_x,
  output int func_params_return_ret,
  // tock_no_params_return() ports
  output int tock_no_params_return_ret,
  // tock_params_no_return() ports
  input int tock_params_no_return_x,
  // tock_params_return() ports
  input int tock_params_return_x,
  output int tock_params_return_ret,
  // tock_calls_funcs1() ports
  input int tock_calls_funcs1_x,
  // tock_calls_funcs2() ports
  input int tock_calls_funcs2_x,
  // tock_calls_tock() ports
  input int tock_calls_tock_x,
  output int tock_calls_tock_ret,
  // tick_params() ports
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

  always_comb begin : func_no_params_return
    func_no_params_return_ret = 1;
  end

  always_comb begin : func_params_return
    func_params_return_ret = func_params_return_x + 1;
  end

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

  always_comb begin : tock_calls_funcs1
    my_sig5a = 12 + my_func5(tock_calls_funcs1_x);
  end

  always_comb begin : tock_calls_funcs2
    my_sig5b = 2 + my_func5(tock_calls_funcs2_x - 7);
  end

/*private:*/
  function int my_func5(int x);
    my_func5 = x + 1;
  endfunction
/*public:*/

  always_comb begin : tock_calls_tock
    my_sig6a = 12;
    tock_called_by_tock_x = my_sig6a;
    /*tock_called_by_tock(my_sig6a);*/
    tock_calls_tock_ret = 0;
  end

/*private:*/
  int my_sig6b;
  always_comb begin : tock_called_by_tock
    my_sig6b = tock_called_by_tock_x;
  end
  int tock_called_by_tock_x;
/*public:*/

  //----------

  always_ff @(posedge clock) begin : tick_no_params
    my_reg1 <= my_reg1 + 1;
    tick_called_by_tick(func_called_by_tick(1));
  end

  always_ff @(posedge clock) begin : tick_params
    my_reg2 <= my_reg2 + tick_params_x;
  end

/*private:*/
  int my_reg3;
  task automatic tick_called_by_tick(int x);
    my_reg3 <= my_reg3 + x;
  endtask

  function int func_called_by_tick(int x);
    func_called_by_tick = x + 7;
  endfunction

/*public:*/

  always_comb begin : tock_only_calls_private_tick
    tick_private_x = 17;
    /*tick_private(17);*/
  end

/*private:*/
  int my_reg4;
  always_ff @(posedge clock) begin : tick_private
    my_reg4 <= my_reg4 + tick_private_x;
  end
  int tick_private_x;


endmodule
