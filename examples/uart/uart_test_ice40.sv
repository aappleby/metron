`ifndef UART_BENCH_SV
`define UART_BENCH_SV
`default_nettype none

`include "uart_top.sv"
`include "SB_PLL40_CORE.v"

//==============================================================================

module uart_ice40(
  input logic CLK,

  // Serial port to host
  output logic SER_DCDn,
  output logic SER_DSRn,
  input  logic SER_DTRn,
  input  logic SER_RTSn,
  output logic SER_CTSn,
  output logic SER_TX,
  input  logic SER_RX,

  // On-board LEDs
  output logic [7:0] LEDS,

  // Top pin row connection to logic analyser
  output logic LOGIC7
);

  logic pll_clk;
  logic rst_n;

  /**
   * PLL configuration
   *
   * This Verilog module was generated automatically
   * using the icepll tool from the IceStorm project.
   * Use at your own risk.
   *
   * Given input frequency:        12.000 MHz
   * Requested output frequency:   24.000 MHz
   * Achieved output frequency:    24.000 MHz
   */
  SB_PLL40_CORE #(
                  .FEEDBACK_PATH("SIMPLE"),
                  .DIVR(4'b0000),         // DIVR =  0
                  .DIVF(7'b0111111),      // DIVF = 63
                  .DIVQ(3'b101),          // DIVQ =  5
                  .FILTER_RANGE(3'b001)   // FILTER_RANGE = 1
          ) uut (
                  .LOCK(rst_n),
                  .RESETB(1'b1),
                  .BYPASS(1'b0),
                  .REFERENCECLK(CLK),
                  .PLLOUTCORE(pll_clk)
                  );


  localparam pll_clk_rate   = 24000000;
  localparam ser_clk_rate   =     1200;
  localparam cycles_per_bit = pll_clk_rate / ser_clk_rate;

  //logic ser_tx;
  logic o_serial;
  logic[7:0] o_data;
  logic o_valid;
  logic o_done;
  logic[31:0] o_sum;

  uart_top #(.cycles_per_bit(cycles_per_bit), .repeat_msg(1)) dut
  (
    pll_clk,
    o_serial,
    o_data,
    o_valid,
    o_done,
    o_sum,
    rst_n
  );

  always_comb begin
    SER_TX = o_serial;
    LOGIC7 = o_serial;
    LEDS = o_valid ? o_data : 0;
  end

endmodule

//==============================================================================

`endif
