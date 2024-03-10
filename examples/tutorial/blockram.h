#include "metron/metron_tools.h"

//------------------------------------------------------------------------------

class Blockram {
public:
  Blockram() {
    readmemh("blockram.hex", memory_, 0, 255);
  }

  logic<8> get_data() const {
    return data_out_;
  }

  void tick(logic<8> address, logic<1> write, logic<8> data_in) {
    if (write) {
      memory_[address] = data_in;
      data_out_ = data_in;
    }
    else {
      data_out_ = memory_[address];
    }
  }

private:
  logic<8> memory_[256];
  logic<8> data_out_;
};
