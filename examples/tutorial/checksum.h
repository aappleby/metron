#include "blockram.h"

class Checksum {
public:

  int state;
  int cursor;
  int sum;

  void tick(int reset) {
    ram.tick(state, 0, 0);

    if (reset) {
      state = 0;
      cursor = 0;
      sum = 0;
    }
    else {
      state = state + 1;
      cursor = cursor + 1;
      sum = sum + 10;
    }
  }

private:
  Blockram ram;
};
