`ifndef UART_TX_H
`define UART_TX_H

`include "metron_tools.sv"

//==============================================================================

module uart_tx
#(parameter int cycles_per_bit = 4)
(
  input logic clock,
  output logic serial_ret,
  output logic cts_ret,
  output logic idle_ret,
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

  function logic serial();
    serial = buffer & 1;
  endfunction
  always_comb serial_ret = serial();

  function logic cts();
    cts = ((cursor == extra_stop_bits) && (cycle == 0)) ||
           (cursor < extra_stop_bits);
  endfunction
  always_comb cts_ret = cts();

  function logic idle();
    idle = (cursor == 0) && (cycle == 0);
  endfunction
  always_comb idle_ret = idle();

  task automatic tick(logic i_rstn, logic[7:0] i_data, logic i_req);
    if (!i_rstn) begin
      cycle <= 0;
      cursor <= 0;
      buffer <= 12'h1FF;
    end else begin
      logic[cycle_bits-1:0] cycle_max;
      logic[cursor_bits-1:0] cursor_max;
      cycle_max = (cycle_bits)'(cycles_per_bit - 1);
      cursor_max = (cursor_bits)'(10 + extra_stop_bits - 1);

      if (/*cursor <= extra_stop_bits*/ extra_stop_bits >= cursor &&
          cycle == 0 && i_req) begin
        // Transmit start
        cycle <= cycle_max;
        cursor <= cursor_max;
        buffer <= i_data << 1;
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
  endtask
  always_ff @(posedge clock) tick(tick_i_rstn, tick_i_data, tick_i_req);

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
