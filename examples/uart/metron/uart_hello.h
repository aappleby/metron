#pragma once
#include "metron_tools.h"

//==============================================================================

template<int repeat_msg = 0>
class uart_hello {
 public:
  uart_hello() {
    readmemh("examples/uart/message.hex", memory, 0, 511);
  }

  //----------------------------------------

  logic<8> tock_data() const { return data; }
  logic<1> tock_req()  const { return state == SEND; }
  logic<1> tock_done() const { return state == DONE; }

  void tock(logic<1> i_rstn, logic<1> i_cts, logic<1> i_idle) {
    tick(i_rstn, i_cts, i_idle);
  }

  //----------------------------------------

 private:
  void tick(logic<1> i_rstn, logic<1> i_cts, logic<1> i_idle) {
    if (!i_rstn) {
      state = WAIT;
      cursor = 0;
    } else {
      data = memory[cursor];
      if (state == WAIT && i_idle) {
        state = SEND;
      } else if (state == SEND && i_cts) {
        if (cursor == b9(message_len - 1)) {
          state = DONE;
        } else {
          cursor = cursor + 1;
        }
      } else if (state == DONE) {
        if (repeat_msg) state = WAIT;
        cursor = 0;
      }
    }
  }

  static const int message_len = 512;
  static const int cursor_bits = clog2(message_len);

  static const int WAIT = 0;
  static const int SEND = 1;
  static const int DONE = 2;

  //enum class state : logic<2>::BASE{WAIT, SEND, DONE};

  logic<2> state;
  logic<cursor_bits> cursor;
  logic<8> memory[512];
  logic<8> data;
};

//==============================================================================
