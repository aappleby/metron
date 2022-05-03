`include"metron_tools.sv"

// Concatenating logics should produce logics with correct <N>

module Module
(
  input logic clock
);
/*public:*/

  initial begin /*Module*/
    /*write("Hello World?\n")*/;
  end

  function void tock1();
    logic a;
    logic[1:0] b;
    logic[2:0] c;
    logic[5:0] d;

    logic a = 1;
    logic[1:0] b = 2;
    logic[2:0] c = 3;

    logic[5:0] d = {a, b, c};
  endfuction

  function void tock2();
    logic[7:0] a;
    logic[7:0] b;
    logic[7:0] c;
    logic[5:0] d;

    logic[7:0] a = 1;
    logic[7:0] b = 2;
    logic[7:0] c = 3;

    logic[5:0] d = {1'(a), 2'(b), 3'(c)};
  endfuction
endmodule;

