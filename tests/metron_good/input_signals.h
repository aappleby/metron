#include "metron/metron_tools.h"

// Public fields that are read by the module but never written become input
// ports.

// this is broken in the new parser because we're not distinguishing
// between input and output signals in submodules...

class Submod {
public:

  logic<8> i_signal;
  logic<8> o_signal;
  logic<8> o_reg_;

  logic<8> tock(logic<8> i_param) {
    o_signal = i_signal + i_param;
    tick();
    return o_signal + 7;
  }

private:

  void tick() {
    o_reg_ = o_reg_ + o_signal;
  }
};

class Module {
public:

  void tock() {
    submod_.i_signal = 12;
    logic<8> submod_return = submod_.tock(13);
    my_sig = submod_return + 3;
    tick();
  }

  logic<8> my_reg_;

private:

  void tick() {
    my_reg_ = my_reg_ + my_sig - 2;
  }

  logic<8> my_sig;

  Submod submod_;
};
