#include "metron/tools/metron_tools.h"

class Module {
public:

  void tock() {
    foo = bar + 1;
    bar = foo + 1;
  }

private:
  int foo;
  int bar;
};
