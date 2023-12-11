#include "metron/metron_tools.h"

// We should be able to pass constructor arguments to submodules.

//----------------------------------------

template<int data_len = 1024, int blarp = 0>
class Module {
public:

  Module(const char* filename = nullptr) {
    if (filename) readmemh(filename, data_);
  }

  void tock(logic<10> new_addr) {
    addr = new_addr;
    tick();
  }

  logic<8> get_data() {
    return out_;
  }

private:
  void tick() {
    out_ = data_[addr];
  }

  logic<10> addr;
  logic<8> data_[data_len];
  logic<8> out_;
};

//----------------------------------------

class Top {
public:
  Top() : submod("examples/uart/message.hex"), derp(7) {
  }

  void tock(logic<10> addr) {
    submod.tock(addr);
  }

  void tick() {
    //mod.tick();
  }

  Module<7777, 8383> submod;
  logic<10> derp;
};

//----------------------------------------
