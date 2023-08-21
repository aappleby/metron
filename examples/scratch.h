#include "metron/tools/metron_tools.h"

//------------------------------------------------------------------------------

struct S {
  int x;
  int y;
};

class Adder {
  int a;
  int b;
};

class Top {
public:

  static const char* some_string = "foobarbaz";

  void foo(logic<7>* a, int b, S c) {}
  void bar() { foo(1+ 3 * 1000, 2); }
  S blarp;
  Adder adder;
};

//------------------------------------------------------------------------------
