`ifndef UART_TX_H
`define UART_TX_H

`include "metron_tools.sv"

//==============================================================================

module uart_tx #(parameter int cycles_per_bit = 4)
(
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
/*public:*/
  initial begin
    bit_delay = 0;
    bit_count = 0;
    output_buffer = 0;
  end

  always_comb begin : get_serial
    get_serial_ret = output_buffer & 1;
  end

  always_comb begin : get_clear_to_send
    get_clear_to_send_ret = ((bit_count == extra_stop_bits) && (bit_delay == 0)) ||
           (bit_count < extra_stop_bits);
  end

  always_comb begin : get_idle
    get_idle_ret = (bit_count == 0) && (bit_delay == 0);
  end

  always_ff @(posedge clock) begin : tick
    if (tick_reset) begin
      bit_delay <= 0;
      bit_count <= 0;
      output_buffer <= 12'h1FF;
    end

    else begin
      // Bit delay
      if (bit_delay != 0) begin
        bit_delay <= bit_delay - 1;
      end

      else if (bit_count <= extra_stop_bits && tick_send_request) begin
        // Transmit start
        bit_delay <= bit_delay_max;
        bit_count <= bit_count_max;
        output_buffer <= tick_send_data << 1;
      end

      else if (bit_count != 0) begin
        // Bit delay done, switch to next bit.
        bit_delay <= bit_delay_max;
        bit_count <= bit_count - 1;
        output_buffer <= (output_buffer >> 1) | 12'h100;
      end
    end
  end

/*private:*/
  // 1 start bit, 8 data bits, 1 stop bit, 7 additional stop bits between
  // messages to guarantee that the receiver can resynchronize with our start
  // bit.
  localparam int extra_stop_bits = 7;

  localparam int bit_delay_width = $clog2(cycles_per_bit);
  localparam int bit_delay_max   = cycles_per_bit - 1;
  logic[bit_delay_width-1:0] bit_delay;

  localparam int bit_count_width = $clog2(10 + extra_stop_bits);
  localparam int bit_count_max   = 10 + extra_stop_bits - 1;
  logic[bit_count_width-1:0] bit_count;

  logic[8:0] output_buffer;
endmodule

//==============================================================================

`endif // UART_TX_H
