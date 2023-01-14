#include "metron_tools.h"

template<int data_len>
class Module {
public:

  Module(const char* filename) {
    readmemh(filename, data);
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
  logic<8> data[data_len];
  logic<8> out;
};
