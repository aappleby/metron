#include "metron_tools.h"

//------------------------------------------------------------------------------

class Blockram {
public:
  Blockram() {
    readmemh("blockram.hex", memory, 0, 255);
  }

  logic<8> get_data() const {
    return data_out;
  }

  void tick(logic<8> address, logic<1> write, logic<8> data_in) {
    if (write) {
      memory[address] = data_in;
      data_out = data_in;
    }
    else {
      data_out = memory[address];
    }
  }

private:
  logic<8> memory[256];
  logic<8> data_out;
};
