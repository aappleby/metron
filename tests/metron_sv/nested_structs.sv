`include "metron/metron_tools.sv"

typedef struct packed {
  logic[7:0] a;
  logic[7:0] b;
  logic[7:0] c;
} InnerStruct;

typedef struct packed {
  InnerStruct x;
  InnerStruct y;
  InnerStruct z;
} OuterStruct;

module Module (
  // global clock
  input logic clock,
  // output registers
  output OuterStruct r,
  output OuterStruct s_
);
/*public:*/


  always_comb begin : tock_func1
    r.x.a = 1;
    r.x.b = 2;
    r.x.c = 3;
  end

  always_ff @(posedge clock) begin : tick_func2
    s_.y.a <= 4;
    s_.y.b <= 5;
    s_.y.c <= 6;
  end

  always_ff @(posedge clock) begin : tick_func3
    s_.z.a <= 7;
    s_.z.b <= 8;
    s_.z.c <= 9;
  end
endmodule
