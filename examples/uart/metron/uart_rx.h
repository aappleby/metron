#ifndef UART_RX_H
#define UART_RX_H

#include "metron/metron_tools.h"

//==============================================================================

template <int cycles_per_bit = 4>
class uart_rx {
public:

  uart_rx() {
    bit_delay_ = 0;
    bit_count_ = 0;
    data_out_ = 0;
    checksum_ = 0;
  }

  // Our output is valid once we've received 8 bits.
  logic<1> get_valid() const {
    return bit_count_ == 8;
  }

  // The most recent data byte received.
  logic<8> get_data_out() const {
    return data_out_;
  }

  // The checksum of all bytes received so far.
  logic<32> get_checksum() const {
    return checksum_;
  }

  void tick(
    logic<1> reset,  // Top-level reset signal
    logic<1> serial) // Serial input from the transmitter
  {
    if (reset) {
      bit_delay_ = bit_delay_max;
      bit_count_ = bit_count_max;
      data_out_ = 0;
      checksum_ = 0;
    }
    else {

      // If we're waiting for the next bit to arrive, keep waiting until our
      // bit delay counter runs out.
      if (bit_delay_ < bit_delay_max) {
        bit_delay_ = bit_delay_ + 1;
      }

      // We're done waiting for a bit. If we have bits left to receive, shift
      // them into the top of the output register.
      else if (bit_count_ < bit_count_max) {
        logic<8> new_output = (serial << 7) | (data_out_ >> 1);

        // If that was the last data bit, add the finished byte to our checksum.
        if (bit_count_ == 7) {
          checksum_ = checksum_ + new_output;
        }

        // Move to the next bit and reset our delay counter.
        bit_delay_ = 0;
        bit_count_ = bit_count_ + 1;
        data_out_ = new_output;
      }

      // We're not waiting for a bit and we finished receiving the previous
      // byte. Wait for the serial line to go low, which signals the start of
      // the next byte.
      else if (serial == 0) {
        bit_delay_ = 0;
        bit_count_ = 0;
      }
    }
  }

 private:
  // We wait for cycles_per_bit cycles
  static const int bit_delay_width = clog2(cycles_per_bit);
  static const int bit_delay_max = cycles_per_bit - 1;
  logic<bit_delay_width> bit_delay_;

  // Our serial data format is 8n1, which is short for "one start bit, 8 data
  // bits, no parity bit, one stop bit". If bit_count_ == 1, we're only waiting
  // on the stop bit.
  static const int bit_count_max = 9;
  static const int bit_count_width = clog2(bit_count_max);
  logic<bit_count_width> bit_count_;

  // The received byte
  logic<8> data_out_;

  // The checksum of all bytes received so far.
  logic<32> checksum_;
};

//==============================================================================

#endif // UART_RX_H
