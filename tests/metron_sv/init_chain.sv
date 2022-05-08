`include "metron_tools.sv"

// I don't know why you would want to do this, but it should work.

module Module
(
  input logic clock
);
/*public:*/
  initial begin
    init1();
  end

  function void tock();
    /*tick()*/;
  endfunction
  always_comb tock();

/*private:*/

  function void tick();
    reg1 <= reg1 + 1;
    reg2 <= reg2 + 1;
    reg3 <= reg3 + 1;
    reg4 <= reg4 + 1;
    reg5 <= reg5 + 1;
  endfunction
  always_ff @(posedge clock) tick();

  logic[7:0] reg1;
  logic[7:0] reg2;
  logic[7:0] reg3;
  logic[7:0] reg4;
  logic[7:0] reg5;

  function void init1();
    reg1 = 1;
    init2();
  endfunction

  function void init2();
    reg2 = 2;
    init3();
  endfunction

  function void init3();
    reg3 = 3;
    init4();
  endfunction

  function void init4();
    reg4 = 4;
    init5();
  endfunction

  function void init5();
    reg5 = 5;
  endfunction

endmodule
