// Yosys breaks if you refer to fields of a local struct in an always_ff

typedef struct packed {
  logic[7:0] field;
} my_struct;

module Module (input logic clock);
  always_ff @ (posedge clock) begin
    my_struct s;
    s.field = 1;
  end
endmodule
