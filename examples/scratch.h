#include "metron/metron_tools.h"

class Module {
public:

  int tock_calls_tock(int x) {
    tock_called_by_tock(x);
    return 0;
  }

private:
  int my_sig6b;
  void tock_called_by_tock(int x) {
    my_sig6b = x;
  }
};
