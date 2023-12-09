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
    submodule__tock_dx = 1;
    submodule__tock_dy = 2;
    /*submodule_.tock(1, 2);*/
    tock_ret = submodule__my_width + submodule__my_height;
  end

  Submod #(
    // Template Parameters
    .WIDTH(10),
    .HEIGHT(11)
  ) submodule_(
    // Output signals
    .my_width(submodule__my_width),
    .my_height(submodule__my_height),
    // tock() ports
    .tock_dx(submodule__tock_dx),
    .tock_dy(submodule__tock_dy)
  );
  logic[10-1:0] submodule__tock_dx;
  logic[11-1:0] submodule__tock_dy;
  logic[10-1:0] submodule__my_width;
  logic[11-1:0] submodule__my_height;
endmodule
