`include "metron_tools.sv"

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
    tock_ret = 0;
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    reg1 <= reg1 + 1;
    reg2 <= reg2 + 1;
    reg3 <= reg3 + 1;
    reg4 <= reg4 + 1;
    reg5 <= reg5 + 1;
  end

  logic[7:0] reg1;
  logic[7:0] reg2;
  logic[7:0] reg3;
  logic[7:0] reg4;
  logic[7:0] reg5;

  task automatic init1();
    reg1 = 1;
    init2();
  endtask

  task automatic init2();
    reg2 = 2;
    init3();
  endtask

  task automatic init3();
    reg3 = 3;
    init4();
  endtask

  task automatic init4();
    reg4 = 4;
    init5();
  endtask

  task automatic init5();
    reg5 = 5;
  endtask

endmodule
