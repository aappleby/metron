#ifndef UART_RX_H
#define UART_RX_H

#include "metron_tools.h"

//==============================================================================

template <int cycles_per_bit = 4>
class uart_rx {
public:
  logic<1> valid() {
    return _cursor == 1;
  }

  logic<8> buffer() {
    return _buffer;
  }

  logic<32> sum() {
    return _sum;
  }

  void tick(logic<1> i_rstn, logic<1> i_serial) {
    if (!i_rstn) {
      _cycle = 0;
      _cursor = 0;
      _buffer = 0;
      _sum = 0;
    } else {
      if (_cycle != 0) {
        _cycle = _cycle - 1;
      } else if (_cursor != 0) {
        logic<8> temp = (i_serial << 7) | (_buffer >> 1);
        if (_cursor - 1 == 1) _sum = _sum + temp;
        _cycle = cycle_max;
        _cursor = _cursor - 1;
        _buffer = temp;
      } else if (i_serial == 0) {
        _cycle = cycle_max;
        _cursor = cursor_max;
      }
    }
  }

 private:
  static const int cycle_bits = clog2(cycles_per_bit);
  static const int cycle_max = cycles_per_bit - 1;
  static const int cursor_max = 9;
  static const int cursor_bits = clog2(cursor_max);

  logic<cycle_bits> _cycle;
  logic<cursor_bits> _cursor;
  logic<8> _buffer;
  logic<32> _sum;
};

//==============================================================================

#endif // UART_RX_H
