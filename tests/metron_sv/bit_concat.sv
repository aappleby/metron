`include "metron_tools.sv"

// Concatenating logics should produce logics with correct <N>

module Module
(
  input logic clock
);
/*public:*/

  initial begin
    $write("Hello World?\n");
  end

  function void tock1();
    logic a;
    logic[1:0] b;
    logic[2:0] c;
    logic[5:0] d;
    a = 1;
    b = 2;
    c = 3;

    d = {a, b, c};
  endfunction

  function void tock2();
    logic[7:0] a;
    logic[7:0] b;
    logic[7:0] c;
    logic[5:0] d;
    a = 1;
    b = 2;
    c = 3;

    d = {1'(a), 2'(b), 3'(c)};
  endfunction

  always_comb begin
    tock1();
    tock2();
  end


  always_ff @(posedge clock) begin
  end

endmodule
