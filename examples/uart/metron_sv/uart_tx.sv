`ifndef UART_TX_H
`define UART_TX_H

`include "metron_tools.sv"

//==============================================================================

module uart_tx
#(parameter int cycles_per_bit = 4)
(
  // global clock
  input logic clock,
  // serial() ports
  output logic serial_ret,
  // cts() ports
  output logic cts_ret,
  // idle() ports
  output logic idle_ret,
  // tick() ports
  input logic tick_i_rstn,
  input logic[7:0] tick_i_data,
  input logic tick_i_req
);
/*public:*/
  initial begin
    cycle = 0;
    cursor = 0;
    buffer = 0;
  end

  //----------------------------------------

  always_comb begin : serial
    serial_ret = buffer & 1;
  end

  always_comb begin : cts
    cts_ret = ((cursor == extra_stop_bits) && (cycle == 0)) ||
           (cursor < extra_stop_bits);
  end

  always_comb begin : idle
    idle_ret = (cursor == 0) && (cycle == 0);
  end

  always_ff @(posedge clock) begin : tick
    if (!tick_i_rstn) begin
      cycle <= 0;
      cursor <= 0;
      buffer <= 12'h1FF;
    end else begin
      logic[cycle_bits-1:0] cycle_max;
      logic[cursor_bits-1:0] cursor_max;
      cycle_max = (cycle_bits)'(cycles_per_bit - 1);
      cursor_max = (cursor_bits)'(10 + extra_stop_bits - 1);

      if (/*cursor <= extra_stop_bits*/ extra_stop_bits >= cursor &&
          cycle == 0 && tick_i_req) begin
        // Transmit start
        cycle <= cycle_max;
        cursor <= cursor_max;
        buffer <= tick_i_data << 1;
      end else if (cycle != 0) begin
        // Bit delay
        cycle <= cycle - 1;
        cursor <= cursor;
        buffer <= buffer;
      end else if (cursor != 0) begin
        // Bit delay done, switch to next bit.
        cycle <= cycle_max;
        cursor <= cursor - 1;
        buffer <= (buffer >> 1) | 12'h100;
      end
    end
  end

  //----------------------------------------
/*private:*/
  // 1 start bit, 8 data bits, 1 stop bit, 7 additional stop bits to guarantee
  // that recevier can resync between messages
  localparam int extra_stop_bits = 7;
  localparam int cycle_bits = $clog2(cycles_per_bit);
  localparam int cursor_bits = $clog2(10 + extra_stop_bits);

  logic[cycle_bits-1:0] cycle;
  logic[cursor_bits-1:0] cursor;
  logic[8:0] buffer;
endmodule

//==============================================================================

`endif // UART_TX_H
