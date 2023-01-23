`include "metron_tools.sv"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal
// verilator lint_off unusedparam

module test_reg (
  // global clock
  input logic clock,
  // output registers
  output logic[31:0] data,
  // tick() ports
  input logic tick_wren,
  input logic[31:0] tick_new_data
);
  parameter addr_mask = 32'hF0000000;
  parameter addr_tag = 32'h00000000;

/*public:*/

  parameter init = 0;
  initial begin
    data = init;
  end

  always_ff @(posedge clock) begin : tick
    if (tick_wren) begin
      data <= tick_new_data;
    end
  end

endmodule

module test_top (
  // global clock
  input logic clock
);
/*public:*/

  always_comb begin : tock
    debug_reg_tick_wren = 1;
    debug_reg_tick_new_data = 32'h12345678;
  end

  test_reg #(
    // Template Parameters
    .addr_mask(32'hF0000000)
    // Constructor Parameters
  ) debug_reg(
    // Global clock
    .clock(clock),
    // Output registers
    .data(debug_reg_data),
    // tick() ports
    .tick_wren(debug_reg_tick_wren),
    .tick_new_data(debug_reg_tick_new_data)
  );
  logic debug_reg_tick_wren;
  logic[31:0] debug_reg_tick_new_data;
  logic[31:0] debug_reg_data;

endmodule

// verilator lint_on unusedsignal
// verilator lint_off unusedparam
//------------------------------------------------------------------------------
