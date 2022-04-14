#pragma once
#include "metron_tools.h"
#include "uart_hello.h"
#include "uart_rx.h"
#include "uart_tx.h"

//==============================================================================

template <int cycles_per_bit = 3, int repeat_msg = 0>
class uart_top {
 public:
  uart_top() {
    write("uart_top.init()\n");
  }

  //----------------------------------------

  logic<1>  tock_serial() { return tx.tock_serial(); }
  logic<8>  tock_data()   { return rx.tock_buffer(); }
  logic<1>  tock_valid()  { return rx.tock_valid(); }
  logic<1>  tock_done()   { return hello.tock_done() && tx.tock_idle(); }
  logic<32> tock_sum()    { return rx.tock_sum(); }

  void tock(logic<1> i_rstn) {
    logic<8> hello_data = hello.tock_data();
    logic<1> hello_req = hello.tock_req();

    rx.tock(i_rstn, tx.tock_serial());
    hello.tock(i_rstn, tx.tock_cts(), tx.tock_idle());
    tx.tock(i_rstn, hello_data, hello_req);
  }

  //----------------------------------------

 private:
  uart_hello<repeat_msg> hello;
  uart_tx<cycles_per_bit> tx;
  uart_rx<cycles_per_bit> rx;
};

//==============================================================================
