#include "metron/tools/metron_tools.h"

//------------------------------------------------------------------------------

class UartRX {
public:
  void tick(int a, int b, int c) {
  }
};

//------------------------------------------------------------------------------

class UartTX {
public:
  UartRX rx;
};

//------------------------------------------------------------------------------

class Module {
public:
  void tick(int a, int b, int c = 12) {
    tx.rx.tick(1, 2, 3);
  }

  void shift_input() {
    dec_shift();
  }

  void dec_shift() {
  }

  UartTX tx;
};

//------------------------------------------------------------------------------
