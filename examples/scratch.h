#ifndef UART_RX_H
#define UART_RX_H

#include "metron_tools.h"

//==============================================================================

template <int cycles_per_bit = 4>
class uart_rx {
public:

  void tick()
  {
    bit_delay = 2;
    bit_count = bit_count_max;
  }

 private:
  static const int bit_count_max = 9;
  logic<32> bit_delay;
  logic<32> bit_count;
};

//==============================================================================

#endif // UART_RX_H
