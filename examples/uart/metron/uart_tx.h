#ifndef UART_TX_H
#define UART_TX_H

#include "metron/metron_tools.h"

//==============================================================================

template <int cycles_per_bit = 4>
class uart_tx {
public:
  uart_tx() {
    bit_delay = bit_delay_max;
    bit_count = bit_count_max;
    output_buffer = 0;
  }

  // The actual bit of data we're sending to the serial port.
  logic<1> get_serial() const {
    return output_buffer & 1;
  }

  // True if the transmitter is ready to accept another byte.
  logic<1> get_clear_to_send() const {
    return ((bit_count == bit_count_done) && (bit_delay == bit_delay_max)) ||
           (bit_count > bit_count_done);
  }

  // True if the transmitter has sent the message plus the extra stop bits.
  logic<1> get_idle() const {
    return (bit_count == bit_count_max) && (bit_delay == bit_delay_max);
  }

  void tick(logic<1> reset, logic<8> send_data, logic<1> send_request) {
    if (reset) {
      bit_delay = bit_delay_max;
      bit_count = bit_count_max;
      output_buffer = 0x1FF;
    }

    else {
      // If we've just sent a bit, wait for the delay to expire before sending
      // another.
      if (bit_delay < bit_delay_max) {
        bit_delay = bit_delay + 1;
      }

      // The bit delay is done. If we have more bits to send, shift our output
      // buffer over and append a stop bit.
      else if (bit_count < bit_count_done) {
        bit_delay = 0;
        bit_count = bit_count + 1;
        output_buffer = (output_buffer >> 1) | 0x100;
      }

      // If we don't have any more bits to send, check for a new send request.
      else if (send_request) {
        bit_delay = 0;
        bit_count = 0;
        // We shift the new byte left by one so that the low 0 bit in the output
        // buffer serves as the start bit for the next byte.
        output_buffer = send_data << 1;
      }

      // If there was no send request, keep sending extra stop bits until we've
      // sent enough.
      else if (bit_count < bit_count_max) {
        bit_delay = 0;
        bit_count = bit_count + 1;
      }
    }
  }

private:

  // We wait {cycles_per_bit} cycles between sending bits.
  static const int bit_delay_width = clog2(cycles_per_bit);
  static const int bit_delay_max   = cycles_per_bit - 1;
  logic<bit_delay_width> bit_delay;

  // We send 1 start bit, 8 data bits, and 1 stop bit per byte = 10 bits per
  // byte total. We also send 7 additional stop bits between messages to
  // guarantee that the receiver can resynchronize with our start bit.

  static const int bit_count_done  = 10;
  static const int extra_stop_bits = 7;
  static const int bit_count_width = clog2(10 + extra_stop_bits);
  static const int bit_count_max   = bit_count_done + extra_stop_bits;
  logic<bit_count_width> bit_count;

  // Our output buffer is 9 (not 8) bits wide so that the low bit can serve as
  // our start bit.
  logic<9> output_buffer;
};

//==============================================================================

#endif // UART_TX_H
