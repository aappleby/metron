`include "metron/metron_tools.sv"

// Templates can be used for module parameters

module Submod (
  // output signals
  output logic[WIDTH-1:0] my_width,
  output logic[HEIGHT-1:0] my_height,
  // tock() ports
  input logic[WIDTH-1:0] tock_dx,
  input logic[HEIGHT-1:0] tock_dy
);
  parameter WIDTH = 123;
  parameter HEIGHT = 456;
/*public:*/

  always_comb begin : tock
    my_width = WIDTH'(100) + tock_dx;
    my_height = HEIGHT'(200) + tock_dy;
  end

endmodule

module Module (
  // tock() ports
  output logic[19:0] tock_ret
);
/*public:*/

  always_comb begin : tock
    submod_tock_dx = 1;
    submod_tock_dy = 2;
    /*submod.tock(1, 2);*/
    tock_ret = submod_my_width + submod_my_height;
  end

  Submod #(
    // Template Parameters
    .WIDTH(10),
    .HEIGHT(11)
  ) submod(
    // Output signals
    .my_width(submod_my_width),
    .my_height(submod_my_height),
    // tock() ports
    .tock_dx(submod_tock_dx),
    .tock_dy(submod_tock_dy)
  );
  logic[10-1:0] submod_tock_dx;
  logic[11-1:0] submod_tock_dy;
  logic[10-1:0] submod_my_width;
  logic[11-1:0] submod_my_height;
endmodule
