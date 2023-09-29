#ifndef UART_HELLO_H
#define UART_HELLO_H

#include "metron/metron_tools.h"

//==============================================================================

template <int repeat_msg = 0>
class uart_hello {
public:
  uart_hello() {
    readmemh("examples/uart/message.hex", memory, 0, 511);
  }

  // The byte of data we want transmitted is always the one at the cursor.
  logic<8> get_data() const {
    return memory[cursor];
  }

  // True if we want to transmit a byte
  logic<1> get_request() const {
    return state == SEND;
  }

  // True if we've transmitted the whole message.
  logic<1> get_done() const {
    return state == DONE;
  }

  void tick(
    logic<1> reset,          // Top-level reset signal.
    logic<1> clear_to_send,  // True if the transmitter can accept an input byte
    logic<1> idle)           // True if the transmitter is idle
  {
    // In reset we're always in WAIT state with the message cursor set to
    // the start of the message buffer.
    if (reset) {
      state = WAIT;
      cursor = 0;
    }

    else {

      // If we're waiting for the transmitter to be free and it's told us that
      // it's idle, go to SEND state.
      if (state == WAIT && idle) {
        state = SEND;
      }

      // If we're currently sending a message and the transmitter is ready to
      // accept another byte,
      else if (state == SEND && clear_to_send) {
        // either go to DONE state if we're about to send the last character of
        // the message
        if (cursor == message_len - 1) {
          state = DONE;
        }

        // or just advance the message cursor.
        else {
          cursor = cursor + 1;
        }
      }

      // If we've finished transmitting, reset the message cursor and either go
      // back to WAIT state if we want to re-transmit or just stay in DONE
      // otherwise.
      else if (state == DONE) {
        cursor = 0;
        if (repeat_msg) state = WAIT;
      }
    }
  }

private:
  static const int message_len = 512;
  static const int cursor_bits = clog2(message_len);

  static const int WAIT = 0; // Waiting for the transmitter to be free
  static const int SEND = 1; // Sending the message buffer
  static const int DONE = 2; // Message buffer sent
  logic<2> state;            // One of the above states

  logic<8> memory[512];      // The buffer preloaded with our message
  logic<cursor_bits> cursor; // Index into the message buffer of the _next_ character to transmit
};

//==============================================================================

#endif // UART_HELLO_H
