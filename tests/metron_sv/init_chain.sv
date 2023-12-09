`include "metron/metron_tools.sv"

// I don't know why you would want to do this, but it should work.

module Module (
  // global clock
  input logic clock,
  // tock() ports
  output int tock_ret
);
/*public:*/
  initial begin
    init1();
  end

  always_comb begin : tock
    /*tick();*/
    tock_ret = 0;
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    reg1_ <= reg1_ + 1;
    reg2_ <= reg2_ + 1;
    reg3_ <= reg3_ + 1;
    reg4_ <= reg4_ + 1;
    reg5_ <= reg5_ + 1;
  end

  logic[7:0] reg1_;
  logic[7:0] reg2_;
  logic[7:0] reg3_;
  logic[7:0] reg4_;
  logic[7:0] reg5_;

  function void init1();
    reg1_ = 1;
    init2();
  endfunction

  function void init2();
    reg2_ = 2;
    init3();
  endfunction

  function void init3();
    reg3_ = 3;
    init4();
  endfunction

  function void init4();
    reg4_ = 4;
    init5();
  endfunction

  function void init5();
    reg5_ = 5;
  endfunction

endmodule
