`include "metron_tools.sv"

// Templates can be used for module parameters

module Submod
#(parameter int WIDTH,parameter  int HEIGHT = 2)
(
  input logic clock,
  output logic[WIDTH-1:0] my_width,
  output logic[HEIGHT-1:0] my_height
);
/*public:*/

  always_comb begin /*tock*/
    my_width = (WIDTH)'(100);
    my_height = (HEIGHT)'(200);
  end

endmodule

module Module
(
  input logic clock
);
/*public:*/

  always_comb begin /*tock*/
    logic[19:0] foo;
    /*submodule.tock()*/;
    foo = submodule_my_width + submodule_my_height;
  end

  Submod #(10,11) submodule(
    // Inputs
    .clock(clock),
    // Outputs
    .my_width(submodule_my_width),
    .my_height(submodule_my_height)
  );
  logic[10-1:0] submodule_my_width;
  logic[11-1:0] submodule_my_height;

endmodule
