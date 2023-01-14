#include "metron_tools.h"

// We should be able to pass constructor arguments to submodules.

//----------------------------------------

class Module {
public:

  Module(const char* filename = nullptr) {
    if (filename) readmemh(filename, data);
  }

  void tock(logic<10> addr_) {
    addr = addr_;
  }

  void tick() {
    out = data[addr];
  }

  logic<8> get_data() {
    return out;
  }

private:
  logic<10> addr;
  logic<8> data[1024];
  logic<8> out;
};

//----------------------------------------

class Top {
public:
  Top() : mod("hello.hex") {
  }

  void tock(logic<10> addr) {
    mod.tock(addr);
  }

  void tick() {
    mod.tick();
  }

  Module mod;
};

//----------------------------------------
