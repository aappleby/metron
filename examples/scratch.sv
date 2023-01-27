`include "metron_tools.sv"

// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

typedef struct packed {
  logic[7:0] a;
  logic[7:0] b;
  logic[7:0] c;
} InnerStruct;

module Submodule (
);
/*public:*/
endmodule

module Module (
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] a,
  output logic[7:0] b,
  output logic[7:0] c,
  // func1() ports
  input InnerStruct func1_is,
  input logic[7:0] func1_derp
);
/*public:*/

  Submodule sm(
  );



  always_ff @(posedge clock) begin : func1
    sm.d = a;
    sm.e = b;
    sm.f = c;
    a <= func1_is.c + func1_derp;
    b <= func1_is.b + func1_derp;
    c <= func1_is.a + func1_derp;
  end
endmodule
