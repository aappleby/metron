`ifndef UART_RX_H
`define UART_RX_H

`include "metron/metron_tools.sv"

//==============================================================================

module uart_rx (
  // global clock
  input logic clock,
  // get_valid() ports
  output logic get_valid_ret,
  // get_data_out() ports
  output logic[7:0] get_data_out_ret,
  // get_checksum() ports
  output logic[31:0] get_checksum_ret,
  // tick() ports
  input logic tick_reset,
  input logic tick_serial
);
  parameter cycles_per_bit = 4;
/*public:*/

  // Our output is valid once we've received 8 bits.
  always_comb begin : get_valid
    get_valid_ret = bit_count == 8;
  end

  // The most recent data byte received.
  always_comb begin : get_data_out
    get_data_out_ret = data_out;
  end

  // The checksum of all bytes received so far.
  always_comb begin : get_checksum
    get_checksum_ret = checksum;
  end

  always_ff @(posedge clock) begin : tick // Serial input from the transmitter

    if (tick_reset) begin
      bit_delay <= bit_delay_max;
      bit_count <= bit_count_max;
      data_out <= 0;
      checksum <= 0;
    end
    else begin

      // If we're waiting for the next bit to arrive, keep waiting until our
      // bit delay counter runs out.
      if (bit_delay < bit_delay_max) begin
        bit_delay <= bit_delay + 1;
      end

      // We're done waiting for a bit. If we have bits left to receive, shift
      // them into the top of the output register.
      else if (bit_count < bit_count_max) begin
        logic[7:0] new_output;
        new_output = (tick_serial << 7) | (data_out >> 1);

        // If that was the last data bit, add the finished byte to our checksum.
        if (bit_count == 7) begin
          checksum <= checksum + new_output;
        end

        // Move to the next bit and reset our delay counter.
        bit_delay <= 0;
        bit_count <= bit_count + 1;
        data_out <= new_output;
      end

      // We're not waiting for a bit and we finished receiving the previous
      // byte. Wait for the serial line to go low, which signals the start of
      // the next byte.
      else if (tick_serial == 0) begin
        bit_delay <= 0;
        bit_count <= 0;
      end
    end
  end

 /*private:*/
  // We wait for cycles_per_bit cycles
  localparam /*static*/ /*const*/ int bit_delay_width = $clog2(cycles_per_bit);
  localparam /*static*/ /*const*/ int bit_delay_max = cycles_per_bit - 1;
  logic[bit_delay_width-1:0] bit_delay;

  // Our serial data format is 8n1, which is short for "one start bit, 8 data
  // bits, no parity bit, one stop bit". If bit_count == 1, we're only waiting
  // on the stop bit.
  localparam /*static*/ /*const*/ int bit_count_max = 9;
  localparam /*static*/ /*const*/ int bit_count_width = $clog2(bit_count_max);
  logic[bit_count_width-1:0] bit_count;

  // The received byte
  logic[7:0] data_out;

  // The checksum of all bytes received so far.
  logic[31:0] checksum;
endmodule

//==============================================================================

`endif // UART_RX_H
