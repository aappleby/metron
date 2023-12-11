#include "metron/metron_tools.h"

class Module {
public:
  Module() {
    counter_ = 0;
  }

  logic<1> done() {
    return counter_ >= 100;
  }

  logic<32> result() {
    return counter_;
  }

  void tock() {
    tick();
  }

private:

  void tick() {
    counter_ = counter_ + 1;
  }

  logic<32> counter_;

};
