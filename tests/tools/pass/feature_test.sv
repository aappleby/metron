
module Derp
(
  input logic clock,
  input int foo,
  output int bar
);

  function func_no_params_no_return();
  endfunction

  function int func_no_params_return();
    func_no_params_return = 1;
  endfunction

  assign bar = 0;





  task automatic mytask();
  endtask

  // calling tasks from always_comb may not get sensitivities
  // always_comb mytask();

  function myfunc();
  endfunction

  // triggering tock functions without return values doesn't work in yosys


  /*
  // v - Ignoring return value of non-void function
  // i - user function 'tock' is being called as a task
  // y - ERROR: Can't resolve task name '\tock'.
  function tock();
  endfunction
  always_comb tock();
  */

  /*
  // y - sytax error
  function void tock();
  endfunction
  always_comb tock();
  */


  /*
  function int tock();
  endfunction
  always_comb begin int x; x = tock(); end
  */


  /*
  int test4;
  always_comb begin
    test4 = 7 + foo;
  end
  */



endmodule


/*
#include "metron/metron_tools.h"

// I don't know why you would want to do this, but it should work.

class Module {
public:
  Module() {
  }

  int my_sig1;
  int my_sig2;
  int my_reg1;
  int my_reg2;

  void func_no_return() {
  }

  int func_return() {
    return 1;
  }

  void tock_no_return() {
    my_sig1 = 12;
    int x = my_sig1;
  }

  int tock_return() {
    my_sig2 = 12;
    return my_sig2;
  }

  void tick_no_params() {
    my_reg1 = my_reg1 + 1;
  }

  void tick_params(int x) {
    my_reg2 = my_reg2 + x;
  }
};
*/
