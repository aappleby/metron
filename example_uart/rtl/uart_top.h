#pragma once
#include "metron_tools.h"
#include "uart_hello.h"
#include "uart_rx.h"
#include "uart_tx.h"

//==============================================================================

template <int cycles_per_bit = 3>
class uart_top {
 public:

  void init() {
    write("uart_top.init()\n");
    hello.init();
  }

  //----------------------------------------

  logic<1> o_serial() const { return tx.o_serial(); }
  logic<8> o_data() const { return rx.buffer; }
  logic<1> o_valid() const { return rx.valid(); }
  logic<1> o_done() const { return hello.o_done && tx.o_idle(); }
  logic<32> o_sum() const { return rx.sum; }

  void tock_update(logic<1> i_rstn) {
    hello.tock();
    rx.tick(i_rstn, tx.o_serial());
    hello.tick(i_rstn, tx.o_cts(), tx.o_idle());
    tx.tick(i_rstn, hello.o_data, hello.o_req);
  }

  //----------------------------------------

 private:
  uart_hello hello;
  uart_tx<cycles_per_bit> tx;
  uart_rx<cycles_per_bit> rx;
};

//==============================================================================
