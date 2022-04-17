#include "metron_tools.h"

// Tock functions can't have more than a single return at the end.

//X Found non-terminal return in method tock

class Module {
public:

  logic<8> tock(logic<8> data) {
    if (data > 10) {
      return 12;
    }
    return 11;
  }
};