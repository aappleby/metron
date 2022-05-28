#ifndef UART_RX_H
#define UART_RX_H

#include "metron_tools.h"

//==============================================================================

template <int cycles_per_bit = 4>
class uart_rx {
public:

  // Our output is valid once we're only waiting on the stop bit.
  logic<1> get_valid() const {
    return bit_count == 1;
  }

  logic<8> get_data_out() const {
    return data_out;
  }

  logic<32> get_checksum() const {
    return checksum;
  }

  void tick(
    logic<1> reset,  // Top-level reset signal
    logic<1> serial) // Serial input from the transmitter
  {
    if (reset) {
      bit_delay = 0;
      bit_count = 0;
      data_out = 0;
      checksum = 0;
    }
    else {

      // If we're waiting for the next bit to arrive, keep waiting until our
      // bit delay counter runs out.
      if (bit_delay != 0) {
        bit_delay = bit_delay - 1;
      }

      // We're done waiting for a bit. If we have bits left to receive, shift
      // them into the top of the output register.
      else if (bit_count != 0) {
        logic<8> new_output = (serial << 7) | (data_out >> 1);
        logic<bit_count_width> new_bit_count = bit_count - 1;

        // If that was the last bit, add the finished byte to our checksum.
        // Note we check for == 1 and not == 0, as the last bit is a "stop" bit.
        if (new_bit_count == 1) {
          checksum = checksum + new_output;
        }

        // Move to the next bit and reset our delay counter.
        bit_delay = bit_delay_max;
        bit_count = new_bit_count;
        data_out = new_output;
      }

      // We're not waiting for a bit and we finished receiving the previous
      // byte. Wait for the serial line to go low, which signals the start of
      // the next byte.
      else if (serial == 0) {
        bit_delay = bit_delay_max;
        bit_count = bit_count_max;
      }
    }
  }

 private:
  // We wait for cycles_per_bit cycles
  static const int bit_delay_width = clog2(cycles_per_bit);
  static const int bit_delay_max = cycles_per_bit - 1;
  logic<bit_delay_width> bit_delay;

  // Our serial data format is 8n1, which is short for "one start bit, 8 data
  // bits, no parity bit, one stop bit". If bit_count == 1, we're only waiting
  // on the stop bit.
  static const int bit_count_max = 9;
  static const int bit_count_width = clog2(bit_count_max);
  logic<bit_count_width> bit_count;

  // The received byte
  logic<8> data_out;

  // The checksum of all bytes received so far.
  logic<32> checksum;
};

//==============================================================================

#endif // UART_RX_H
