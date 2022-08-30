module Module(input logic clock);

  function f();
  endfunction

  //always_comb f();

  //always_ff @(posedge clock) f();

endmodule


/*
module Module;

  function void f();
  endfunction

  always_comb f();

endmodule
*/


/*
module Module;

  function int f();
    f = 1;
  endfunction

  always_comb f();

endmodule
*/
