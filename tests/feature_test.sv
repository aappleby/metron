
module Derp
(
  input int foo,
  output int bar
);

  task mytask();
  endtask

  function myfunc();
  endfunction


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
