#ifndef UART_TX_H
#define UART_TX_H

#include "metron_tools.h"

//==============================================================================

template <int cycles_per_bit = 4>
class uart_tx {
public:
  uart_tx() {
    bit_delay = 0;
    bit_count = 0;
    output_buffer = 0;
  }

  logic<1> get_serial() const {
    return output_buffer & 1;
  }

  logic<1> get_clear_to_send() const {
    return ((bit_count == extra_stop_bits) && (bit_delay == 0)) ||
           (bit_count < extra_stop_bits);
  }

  logic<1> get_idle() const {
    return (bit_count == 0) && (bit_delay == 0);
  }

  void tick(logic<1> reset, logic<8> send_data, logic<1> send_request) {
    if (reset) {
      bit_delay = 0;
      bit_count = 0;
      output_buffer = 0x1FF;
    }

    else {
      // Bit delay
      if (bit_delay != 0) {
        bit_delay = bit_delay - 1;
      }

      else if (bit_count <= extra_stop_bits && send_request) {
        // Transmit start
        bit_delay = bit_delay_max;
        bit_count = bit_count_max;
        output_buffer = send_data << 1;
      }

      else if (bit_count != 0) {
        // Bit delay done, switch to next bit.
        bit_delay = bit_delay_max;
        bit_count = bit_count - 1;
        output_buffer = (output_buffer >> 1) | 0x100;
      }
    }
  }

private:
  // 1 start bit, 8 data bits, 1 stop bit, 7 additional stop bits between
  // messages to guarantee that the receiver can resynchronize with our start
  // bit.
  static const int extra_stop_bits = 7;

  static const int bit_delay_width = clog2(cycles_per_bit);
  static const int bit_delay_max   = cycles_per_bit - 1;
  logic<bit_delay_width> bit_delay;

  static const int bit_count_width = clog2(10 + extra_stop_bits);
  static const int bit_count_max   = 10 + extra_stop_bits - 1;
  logic<bit_count_width> bit_count;

  logic<9> output_buffer;
};

//==============================================================================

#endif // UART_TX_H
