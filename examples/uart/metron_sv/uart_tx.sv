`ifndef UART_TX_H
`define UART_TX_H

`include "metron/metron_tools.sv"

//==============================================================================

module uart_tx (
  // global clock
  input logic clock,
  // get_serial() ports
  output logic get_serial_ret,
  // get_clear_to_send() ports
  output logic get_clear_to_send_ret,
  // get_idle() ports
  output logic get_idle_ret,
  // tick() ports
  input logic tick_reset,
  input logic[7:0] tick_send_data,
  input logic tick_send_request
);
  parameter cycles_per_bit = 4;
/*public:*/
  initial begin
    bit_delay = bit_delay_max;
    bit_count = bit_count_max;
    output_buffer = 0;
  end

  // The actual bit of data we're sending to the serial port.
  always_comb begin : get_serial
    get_serial_ret = output_buffer & 1;
  end

  // True if the transmitter is ready to accept another byte.
  always_comb begin : get_clear_to_send
    get_clear_to_send_ret = ((bit_count == bit_count_done) && (bit_delay == bit_delay_max)) ||
           (bit_count > bit_count_done);
  end

  // True if the transmitter has sent the message plus the extra stop bits.
  always_comb begin : get_idle
    get_idle_ret = (bit_count == bit_count_max) && (bit_delay == bit_delay_max);
  end

  always_ff @(posedge clock) begin : tick
    if (tick_reset) begin
      bit_delay <= bit_delay_max;
      bit_count <= bit_count_max;
      output_buffer <= 12'h1FF;
    end

    else begin
      // If we've just sent a bit, wait for the delay to expire before sending
      // another.
      if (bit_delay < bit_delay_max) begin
        bit_delay <= bit_delay + 1;
      end

      // The bit delay is done. If we have more bits to send, shift our output
      // buffer over and append a stop bit.
      else if (bit_count < bit_count_done) begin
        bit_delay <= 0;
        bit_count <= bit_count + 1;
        output_buffer <= (output_buffer >> 1) | 12'h100;
      end

      // If we don't have any more bits to send, check for a new send request.
      else if (tick_send_request) begin
        bit_delay <= 0;
        bit_count <= 0;
        // We shift the new byte left by one so that the low 0 bit in the output
        // buffer serves as the start bit for the next byte.
        output_buffer <= tick_send_data << 1;
      end

      // If there was no send request, keep sending extra stop bits until we've
      // sent enough.
      else if (bit_count < bit_count_max) begin
        bit_delay <= 0;
        bit_count <= bit_count + 1;
      end
    end
  end

/*private:*/

  // We wait {cycles_per_bit} cycles between sending bits.
  localparam /*static*/ /*const*/ int bit_delay_width = $clog2(cycles_per_bit);
  localparam /*static*/ /*const*/ int bit_delay_max   = cycles_per_bit - 1;
  logic[bit_delay_width-1:0] bit_delay;

  // We send 1 start bit, 8 data bits, and 1 stop bit per byte = 10 bits per
  // byte total. We also send 7 additional stop bits between messages to
  // guarantee that the receiver can resynchronize with our start bit.

  localparam /*static*/ /*const*/ int bit_count_done  = 10;
  localparam /*static*/ /*const*/ int extra_stop_bits = 7;
  localparam /*static*/ /*const*/ int bit_count_width = $clog2(10 + extra_stop_bits);
  localparam /*static*/ /*const*/ int bit_count_max   = bit_count_done + extra_stop_bits;
  logic[bit_count_width-1:0] bit_count;

  // Our output buffer is 9 (not 8) bits wide so that the low bit can serve as
  // our start bit.
  logic[8:0] output_buffer;
endmodule

//==============================================================================

`endif // UART_TX_H
