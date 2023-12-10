`include "metron/metron_tools.sv"

typedef union packed {
  logic[31:0] a;
  logic[31:0] b;
} my_union;

module Module (
  // output signals
  output logic[31:0] a,
  output logic[31:0] b,
  // tock() ports
  input logic[31:0] tock_x,
  input logic[31:0] tock_y
);
/*public:*/

  always_comb begin : tock
    blah.a = tock_x;
    blah.b = tock_y;
  end

  my_union blah;
endmodule
