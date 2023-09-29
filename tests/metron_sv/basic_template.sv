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
    submodule_tock_dx = 1;
    submodule_tock_dy = 2;
    /*submodule.tock(1, 2);*/
    tock_ret = submodule_my_width + submodule_my_height;
  end

  Submod #(
    // Template Parameters
    .WIDTH(10),
    .HEIGHT(11)
  ) submodule(
    // Output signals
    .my_width(submodule_my_width),
    .my_height(submodule_my_height),
    // tock() ports
    .tock_dx(submodule_tock_dx),
    .tock_dy(submodule_tock_dy)
  );
  logic[10-1:0] submodule_tock_dx;
  logic[11-1:0] submodule_tock_dy;
  logic[10-1:0] submodule_my_width;
  logic[11-1:0] submodule_my_height;
endmodule
