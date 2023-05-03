`include "metron/tools/metron_tools.sv"

// Yosys breaks if you refer to fields of a local struct in an always_comb

typedef struct packed {
  logic[7:0] field;
} my_struct;

module Module (input logic clock);
  always_comb begin
    my_struct s;
    s.field = 1;
  end
endmodule
