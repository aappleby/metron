#include "metron_tools.h"

class Module {
public:
  void only_calls_private_tick() {
    private_tick(17);
  }

private:
  int my_reg4;
  void private_tick(int x) {
    my_reg4 = my_reg4 + x;
  }
};
