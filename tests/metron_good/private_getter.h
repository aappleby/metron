#include "metron_tools.h"

// Private getter methods are OK

class Module {
public:

  logic<8> tock() {
    return my_getter();
  }

private:

  logic<8> my_getter() const {
    return 12;
  }

};
