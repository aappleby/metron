#include "metron/tools/metron_tools.h"

class Module {
public:

  void tick() {
    int bar = 5;
    foo++;
    bar++;
  }

private:
  int foo;
};
