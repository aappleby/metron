`include "metron_tools.sv"

// Templates can be used for module parameters

module Submod
#(parameter int WIDTH,parameter  int HEIGHT = 2)
(
  output logic[WIDTH-1:0] my_width,
  output logic[HEIGHT-1:0] my_height
);
/*public:*/

  task  tock();
    my_width = (WIDTH)'(100);
    my_height = (HEIGHT)'(200);
  endtask
  always_comb tock();

endmodule

module Module
(
);
/*public:*/

  task  tock();
    logic[19:0] foo;
    /*submodule.tock*/;
    foo = submodule_my_width + submodule_my_height;
  endtask
  always_comb tock();

  Submod #(10,11) submodule(
    .my_width(submodule_my_width),
    .my_height(submodule_my_height)
  );
  logic[10-1:0] submodule_my_width;
  logic[11-1:0] submodule_my_height;

endmodule
