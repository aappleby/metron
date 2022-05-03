`include"metron_tools.sv"

// Make sure our built-in functions translate.

module Module
(
  input logic clock
);
/*public:*/

  function void tock();
    logic[7:0] src;
    logic[7:0] a;
    logic[7:0] b;
    logic[7:0] e;
    logic[7:0] c;
    logic[7:0] d;

    logic[7:0] src = 100;
    logic[7:0] a = $signed(src);
    logic[7:0] b = $unsigned(src);
    logic[7:0] e = $signed(2'(src));
    logic[7:0] c = $clog2(100);
    logic[7:0] d = 2**(4);
  endfuction

endmodule;

