#include "metron/metron_tools.h"

// Namespaces turn into packages.
// "using" doesn't work in methods right now :/

namespace MyPackage {
  static const int foo = 3;
};

class Module {
public:

  int my_sig;
  int my_reg_;

  int tock_ret;
  void tock() {
    my_sig = MyPackage::foo + 1;
    tock_ret = my_sig;
  }

  void tick() {
    my_reg_ = my_reg_ + MyPackage::foo;
  }
};
