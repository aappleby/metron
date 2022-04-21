#pragma once
#include "metron_tools.h"

//==============================================================================

template<int repeat_msg = 0>
class uart_hello {
 public:
  uart_hello() {
    readmemh("examples/uart/message.hex", _memory, 0, 511);
  }

  //----------------------------------------

  logic<8> data() const { return _data; }
  logic<1> req()  const { return _state == SEND; }
  logic<1> done() const { return _state == DONE; }

  void tick(logic<1> i_rstn, logic<1> i_cts, logic<1> i_idle) {
    if (!i_rstn) {
      _state = WAIT;
      _cursor = 0;
    } else {
      _data = _memory[_cursor];
      if (_state == WAIT && i_idle) {
        _state = SEND;
      } else if (_state == SEND && i_cts) {
        if (_cursor == b9(message_len - 1)) {
          _state = DONE;
        } else {
          _cursor = _cursor + 1;
        }
      } else if (_state == DONE) {
        if (repeat_msg) _state = WAIT;
        _cursor = 0;
      }
    }
  }

  //----------------------------------------

  static const int message_len = 512;
  static const int cursor_bits = clog2(message_len);

  static const int WAIT = 0;
  static const int SEND = 1;
  static const int DONE = 2;

  //enum class state : logic<2>::BASE{WAIT, SEND, DONE};

  logic<2> _state;
  logic<cursor_bits> _cursor;
  logic<8> _memory[512];
  logic<8> _data;
};

//==============================================================================
