`include "metron/metron_tools.sv"

// Using slice<>() to write to sub-sections of a variable should work.
// FIXME This can break tracing....

module test_slice (
  // global clock
  input logic clock,
  // tick() ports
  input logic[7:0] tick_addr,
  input logic[3:0] tick_mask,
  input logic[31:0] tick_wdata
);
/*public:*/

  always_ff @(posedge clock) begin : tick
    if (tick_mask[0]) ram_[tick_addr][ 7: 0] <= 8'(tick_wdata);
    if (tick_mask[1]) ram_[tick_addr][15: 8] <= tick_wdata[15:8];
    if (tick_mask[2]) ram_[tick_addr][23:16] <= tick_wdata[23:16];
    if (tick_mask[3]) ram_[tick_addr][31:24] <= tick_wdata[31:24];
  end

/*private:*/
  logic[31:0] ram_[256];
endmodule
