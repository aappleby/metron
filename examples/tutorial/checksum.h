#include "metron/metron_tools.h"
#include "blockram.h"

//------------------------------------------------------------------------------

class Checksum {
public:

  int get_checksum() const {
    return sum_;
  }

  int get_done() const {
    return cursor_ == 256;
  }

  void tock(int reset) {
    logic<8> data = ram.get_data();
    ram.tick(cursor_, 0, 0);
    tick(reset, data);
  }

private:

  void tick(int reset, logic<8> data) {
    if (reset) {
      cursor_ = 0;
      sum_ = 0;
    }
    else {
      if (cursor_ < 256) {
        cursor_ = cursor_ + 1;
        sum_ = sum_ + data;
      }
    }
  }

  int cursor_;
  int sum_;
  Blockram ram;
};

//------------------------------------------------------------------------------
