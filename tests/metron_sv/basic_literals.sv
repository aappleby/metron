`include "metron_tools.sv"

// Number literals
// don't forget the ' spacers

module Module
(
  input logic clock
);
/*public:*/

  function void tock1();
    logic[31:0] a;
    logic[31:0] b;
    logic[31:0] c;
    logic[31:0] d;
    logic[31:0] e;
    logic[31:0] f;
    logic[31:0] g;
    logic[31:0] h;

    logic[31:0] a = 1'b0;
    logic[31:0] b = 2'b00;
    logic[31:0] c = 3'b000;
    logic[31:0] d = 4'b0000;
    logic[31:0] e = 5'b00000;
    logic[31:0] f = 6'b000000;
    logic[31:0] g = 7'b0000000;
    logic[31:0] h = 8'b00000000;
  endfuction

  function void tock2();
    logic[31:0] a;
    logic[31:0] b;
    logic[31:0] c;
    logic[31:0] d;
    logic[31:0] e;
    logic[31:0] f;
    logic[31:0] g;
    logic[31:0] h;

    logic[31:0] a = 1'b0;
    logic[31:0] b = 2'b0_0;
    logic[31:0] c = 3'b0_00;
    logic[31:0] d = 4'b00_00;
    logic[31:0] e = 5'b00_000;
    logic[31:0] f = 6'b0_000_00;
    logic[31:0] g = 7'b000_0000;
    logic[31:0] h = 8'b0_0_0_0_0_0_0_0;
  endfuction

endmodule

