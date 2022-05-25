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

//------------------------------------------------------------------------------

class Checksum {
public:

  int get_checksum() const {
    return sum;
  }

  int get_done() const {
    return cursor == 256;
  }

  void tock(int reset) {
    logic<8> data = ram.get_data();

    ram.tick(cursor, 0, 0);
    tick(reset, data);
  }

private:

  void tick(int reset, logic<8> data) {
    if (reset) {
      cursor = 0;
      sum = 0;
    }
    else {
      if (cursor < 256) {
        cursor = cursor + 1;
        sum = sum + data;
      }
    }
  }

  int cursor;
  int sum;
  Blockram ram;
};

//------------------------------------------------------------------------------
