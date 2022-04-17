#include "metron_tools.h"

class Module {
public:

  logic<8> tock() {
    static const int thing1 = 1;
    const int thing2 = 2;
    return thing1 + thing2;
  }

};
