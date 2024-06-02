#ifndef UART_TOP_H
#define UART_TOP_H

#include "metron/metron_tools.h"
#include "uart_hello.h"
#include "uart_rx.h"
#include "uart_tx.h"

//==============================================================================

template <int cycles_per_bit = 3, int repeat_msg = 0>
class uart_top {
public:

  uart_top(const char* message_file = "override_me2.hex") : hello_(message_file) {
  }

  // The actual bit of data currently on the transmitter's output
  logic<1> get_serial() const {
    return tx_.get_serial();
  }

  // Returns true if the receiver has a byte in its buffer
  logic<1> get_valid() const {
    return rx_.get_valid();
  }

  // The next byte of data from the receiver
  logic<8> get_data_out() const {
    return rx_.get_data_out();
  }

  // True if the client has sent its message and the transmitter has finished
  // transmitting it.
  logic<1> get_done() const {
    return hello_.get_done() && tx_.get_idle();
  }

  // Checksum of all the bytes received
  logic<32> get_checksum() const {
    return rx_.get_checksum();
  }

  void tock(logic<1> reset) {
    // Grab signals from our submodules before we tick them.
    logic<8> data = hello_.get_data();
    logic<1> request = hello_.get_request();

    logic<1> serial = tx_.get_serial();
    logic<1> ready = tx_.get_ready();
    logic<1> idle = tx_.get_idle();

    // Tick all submodules.
    hello_.tick(reset, ready, idle);
    tx_.tick(reset, data, request);
    rx_.tick(reset, serial);
  }

  //----------------------------------------
private:
  // Our UART client that transmits our "hello world" test message
  uart_hello<repeat_msg>  hello_;
  // The UART transmitter
  uart_tx<cycles_per_bit> tx_;
  // The UART receiver
  uart_rx<cycles_per_bit> rx_;
};

//==============================================================================

#endif // UART_TOP_H
