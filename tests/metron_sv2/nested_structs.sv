`include "metron/tools/metron_tools.sv"

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
  input logic clock
);
/*public:*/


  always_comb begin : tock_func1
    s.x.a = 1;
    s.x.b = 2;
    s.x.c = 3;
  end

  always_ff @(posedge clock) begin : tick_func2
    s.y.a <= 4;
    s.y.b <= 5;
    s.y.c <= 6;
  end

  always_ff @(posedge clock) begin : tick_func3
    s.z.a <= 7;
    s.z.b <= 8;
    s.z.c <= 9;
  end
endmodule
