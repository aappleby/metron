`include "metron_tools.sv"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal
// verilator lint_off undriven

typedef struct packed {
  logic[31:0] a_data;
} tilelink_a;

module block_ram (
  // global clock
  input logic clock,
  // output signals
  output logic[31:0] data,
  // tick() ports
  input tilelink_a tick_tla
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    data <= tla.a_data;
  end

endmodule

// verilator lint_on unusedsignal
// verilator lint_on undriven

//------------------------------------------------------------------------------
