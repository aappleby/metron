#include "metron/metron_tools.h"

// Members whose names have trailing underscores are states.
// Members without trailing underscores are signals.
// We should not require any other special annotations on our modules.

class Module {
public:

  Module() {
    counter_ = 0;
  }

  logic<1> get_even() {
    return is_even;
  }

  void tock(logic<32> inc) {
    is_even = (counter_ & 1) == 0;
    update(inc);
  }

  void update(logic<32> inc) {
    update2(inc);
  }

  void update2(logic<32> inc) {
    counter_ = counter_ + inc;
  }

  logic<32> get_counter() {
    return counter_;
  }

private:

  logic<1>  is_even;
  logic<32> counter_;
};
