#include "metron/metron_tools.h"

class Module {
public:
  Module() {
    counter_ = 0;
    lfsr_ = 0xDEADBEEF;
  }

  logic<1> done() {
    return counter_ >= 100;
  }

  logic<32> result() {
    return lfsr_;
  }

  void tock() {
    tick();
  }

private:

  void tick() {
    counter_ = counter_ + 1;
    logic<1> next_bit = lfsr_[31] ^ lfsr_[21] ^ lfsr_[1] ^ lfsr_[0];
    lfsr_ = cat(lfsr_, next_bit);
  }

  logic<32> counter_;
  logic<32> lfsr_;

};
