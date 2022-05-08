`include "metron_tools.sv"

module Module
(
);
/*public:*/

  function tock_dup1();
    logic a1;
    logic b1;
    a1 = 1'b1;
    b1 = {1 {a1}};
  endfunction
  always_comb tock_dup1();

  function tock_dup4();
    logic  a1;
    logic[3:0]  b1;
    logic[1:0]  a2;
    logic[7:0]  b2;
    logic[2:0]  a3;
    logic[11:0] b3;
    logic[3:0]  a4;
    logic[15:0] b4;
    logic[4:0]  a5;
    logic[19:0] b5;
    logic[5:0]  a6;
    logic[23:0] b6;
    logic[6:0]  a7;
    logic[27:0] b7;
    logic[7:0]  a8;
    logic[31:0] b8;
    a1 = 1'b1;
    b1 = {4 {a1}};

    a2 = 2'b01;
    b2 = {4 {a2}};

    a3 = 3'b001;
    b3 = {4 {a3}};

    a4 = 4'b0001;
    b4 = {4 {a4}};

    a5 = 5'b00001;
    b5 = {4 {a5}};

    a6 = 6'b000001;
    b6 = {4 {a6}};

    a7 = 7'b0000001;
    b7 = {4 {a7}};

    a8 = 8'b00000001;
    b8 = {4 {a8}};
  endfunction
  always_comb tock_dup4();
endmodule
