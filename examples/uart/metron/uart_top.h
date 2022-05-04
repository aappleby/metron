#pragma once
#include "metron_tools.h"
#include "uart_hello.h"
#include "uart_rx.h"
#include "uart_tx.h"

//==============================================================================

template <int cycles_per_bit = 3, int repeat_msg = 0>
class uart_top {
 public:
  logic<1> serial() { return tx.serial(); }
  logic<8> data() { return rx.buffer(); }
  logic<1> valid() { return rx.valid(); }
  logic<1> done() { return hello.done() && tx.idle(); }
  logic<32> sum() { return rx.sum(); }

  void tock(logic<1> i_rstn) {
    logic<8> hello_data = hello.data();
    logic<1> hello_req = hello.req();

    rx.tick(i_rstn, tx.serial());
    hello.tick(i_rstn, tx.cts(), tx.idle());
    tx.tick(i_rstn, hello_data, hello_req);
  }

  //----------------------------------------
 private:
  uart_hello<repeat_msg> hello;
  uart_tx<cycles_per_bit> tx;
  uart_rx<cycles_per_bit> rx;
};

//==============================================================================
