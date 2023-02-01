`include "metron_tools.sv"

// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

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
  // output signals
  output OuterStruct s
);
/*public:*/


  always_comb begin : func1
    s.x.a = 1;
    s.x.b = 2;
    s.x.c = 3;
  end

  always_comb begin : func2
    s.y.a = 4;
    s.y.b = 5;
    s.y.c = 6;
  end

  always_comb begin : func3
    s.z.a = 7;
    s.z.b = 8;
    s.z.c = 9;
  end
endmodule
