`include "metron_tools.sv"

//==============================================================================

module uart_tx
#(parameter int cycles_per_bit = 4)
(
  input logic clock,
  input logic tock_i_rstn,
  input logic[7:0] tock_i_data,
  input logic tock_i_req,
  output logic o_serial,
  output logic o_cts,
  output logic o_idle
);
 /*public:*/
  //----------------------------------------

  always_comb begin o_serial = buffer & 1; end

  always_comb begin
    o_cts = ((cursor == extra_stop_bits) && (cycle == 0)) ||
           (cursor < extra_stop_bits);
  end

  always_comb begin o_idle = (cursor == 0) && (cycle == 0); end

  always_comb begin /*tock*/
    tick_i_rstn = tock_i_rstn;
    tick_i_data = tock_i_data;
    tick_i_req = tock_i_req;
    /*tick(i_rstn, i_data, i_req)*/;
  end

  //----------------------------------------

 /*private:*/
  logic tick_i_rstn;
  logic[7:0] tick_i_data;
  logic tick_i_req;
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

