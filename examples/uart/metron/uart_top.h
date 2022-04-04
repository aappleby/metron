#pragma once
#include "metron_tools.h"
#include "uart_hello.h"
#include "uart_rx.h"
#include "uart_tx.h"

//==============================================================================

template <int cycles_per_bit = 3>
class uart_top {
 public:
  uart_top() {
    write("uart_top.init()\n");
    //hello.init();
  }

  //----------------------------------------

  logic<8>  o_data()  const { return rx.o_buffer(); }
  logic<1>  o_valid() const { return rx.o_valid(); }
  logic<1>  o_done()  const { return hello.o_done() && tx.o_idle(); }
  logic<32> o_sum()   const { return rx.o_sum(); }

  void tock(logic<1> i_rstn) {

    rx.tock(i_rstn, tx.o_serial());
    hello.tock(i_rstn, tx.o_cts(), tx.o_idle());

    logic<8> hello_data = hello.o_data();
    logic<1> hello_req = hello.o_req();
    tx.tock(i_rstn, hello_data, hello_req);
  }

  // FIXME need to deduce task vs function by call site, not return type

  //void glarp(logic<1> i_rstn) {
  //}

  //----------------------------------------

 private:
  uart_hello hello;
  uart_tx<cycles_per_bit> tx;
  uart_rx<cycles_per_bit> rx;
};

//==============================================================================
