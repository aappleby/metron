#include "metron/metron_tools.h"

// Modules can contain other modules.

class Module {
public:

  void tock5() {
    sig5 = sig3 + sig2;
  }

  void tock4() {
    sig4 = sig2 + 1;
  }

  void tock2() {
    sig2 = sig1 + 1;
  }

  void tock1(int x) {
    sig1 = x;
  }

  void tock3() {
    sig3 = sig2 + 1;
  }

  int sig1;
  int sig2;
  int sig3;
  int sig4;
  int sig5;
};
