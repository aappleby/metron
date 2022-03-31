#pragma once
#include "metron_tools.h"

//==============================================================================

class uart_hello {
 public:
  void init() { readmemh("examples/uart/message.hex", memory, 0, 511); }

  //----------------------------------------

  logic<8> o_data() const { return data; }
  logic<1> o_req()  const { return s == state::SEND; }
  logic<1> o_done() const { return s == state::DONE; }

  void tock(logic<1> i_rstn, logic<1> i_cts, logic<1> i_idle) {
    tick(i_rstn, i_cts, i_idle);
  }

  //----------------------------------------

 private:
  void tick(logic<1> i_rstn, logic<1> i_cts, logic<1> i_idle) {
    if (!i_rstn) {
      s = state::WAIT;
      cursor = 0;
    } else {
      data = memory[cursor];
      if (s == state::WAIT && i_idle) {
        s = state::SEND;
      } else if (s == state::SEND && i_cts) {
        if (cursor == b9(message_len - 1)) {
          s = state::DONE;
        } else {
          cursor = cursor + 1;
        }
      } else if (s == state::DONE) {
        // s = state::WAIT;
        cursor = 0;
      }
    }
  }

  static const int message_len = 512;
  static const int cursor_bits = clog2(message_len);

  enum class state : logic<2>::BASE{WAIT, SEND, DONE};

  state s;
  logic<cursor_bits> cursor;
  logic<8> memory[512];
  logic<8> data;
};

//==============================================================================
