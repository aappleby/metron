`include "metron/metron_tools.sv"

typedef union packed {
  logic[31:0] a;
  logic[31:0] b;
} my_union;

module Module (
  // tock() ports
  input logic[31:0] tock_x,
  input logic[31:0] tock_y
);
/*public:*/

  always_comb begin : tock
    blah.a = tock_x;
    blah.b = tock_y;
  end

endmodule
