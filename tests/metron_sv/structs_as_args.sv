`include "metron/metron_tools.sv"

// Passing structs to functions that turn into always_comb or always_ff
// should prepend the function name to the struct name.

//------------------------------------------------------------------------------

typedef struct packed {
  logic[31:0] a_data;
} tilelink_a;

module block_ram (
  // global clock
  input logic clock,
  // unshell() ports
  input tilelink_a unshell_tla,
  output logic[31:0] unshell_ret,
  // tick() ports
  input tilelink_a tick_tla
);
/*public:*/

  always_comb begin : unshell
    unshell_ret = unshell_tla.a_data;
  end

  always_ff @(posedge clock) begin : tick
    data_ <= tick_tla.a_data;
  end

endmodule

//------------------------------------------------------------------------------
