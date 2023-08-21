#include "metron/tools/metron_tools.h"

//------------------------------------------------------------------------------

struct S {
  int x;
  int y;
};

class Top {
public:
  void foo(logic<7>* a, int b, S c) {}
  void bar() { foo(1+ 3 * 1000, 2); }
  S blarp;
};

//------------------------------------------------------------------------------
