#include "metron/metron_tools.h"

// Tock functions can't have more than a single return at the end.

// X Method tock has non-terminal return

class Module {
 public:
  int tock(int data) {
    if (data > 10) {
      return 12;
    }
    return 11;
  }
};
