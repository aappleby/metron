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
  // output signals
  output OuterStruct s
);
/*public:*/


  always_comb begin : func1
    s_x_a = 1;
    s_x_b = 2;
    s_x_c = 3;
  end

  always_comb begin : func2
    s_y_a = 4;
    s_y_b = 5;
    s_y_c = 6;
  end

  always_comb begin : func3
    s_z_a = 7;
    s_z_b = 8;
    s_z_c = 9;
  end
endmodule
