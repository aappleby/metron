`include "metron_tools.sv"

// Yosys doesn't seem to support passing structs as args or returning them from
// functions. :/

typedef struct packed {
  logic[7:0] a;
  logic[7:0] b;
  logic[7:0] c;
  logic[31:0] d[7];
} InnerStruct;

typedef struct packed {
  InnerStruct x;
  InnerStruct y;
  InnerStruct z;
} OuterStruct;

module Mipule (
  // tock() ports
  input logic[6:0] tock_slkdfjlskdj
);
  /*public:*/

  always_comb begin : tock
  end

endmodule

module Module (
  // output registers
  output OuterStruct s,
  // func1() ports
  input logic[6:0] func1_blerp,
  // func2() ports
  input OuterStruct func2_clarnk,
  // func3() ports
  input InnerStruct func3_querz
);
/*public:*/

  Mipule the_mip(
    // tock() ports
    .tock_slkdfjlskdj(the_mip_tock_slkdfjlskdj)
  );
  logic[6:0] the_mip_tock_slkdfjlskdj;




  always_comb begin : func1
    s.x.a = 1;
    s.x.b = 2 + func1_blerp;
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
