#include "metron/metron_tools.h"
#include "blockram.h"

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
