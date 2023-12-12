#include "metron/metron_tools.h"

// Private getter methods are OK

class Module {
public:

  int my_sig;

  int tock_ret;
  void tock() {
    my_sig = my_getter();
    tock_ret = my_sig;
  }

private:

  int my_getter() const {
    return 12;
  }

};
