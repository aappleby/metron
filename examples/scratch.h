#include "metron/tools/metron_tools.h"

class Module {
public:

  void tick(int a, int b, int c = 12) {
    blip::blap::tock(1, 2, 3);

    snip.snap.snock(1, 2, 3);
  }

  void tock(int q, int r, int s) {
  }

};
