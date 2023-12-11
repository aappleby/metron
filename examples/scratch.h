#include "metron/metron_tools.h"

class Module {
public:

  void update(int x = 7) {
    update_tock(x);
    update_tick(x);
  }

  void update_tock(int x) {
    sig = x + 7;
  }

  void update_tick(int x) {
    reg_ = x + 7;
  }

  int sig;
  int reg_;
};
