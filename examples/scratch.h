#include "metron/tools/metron_tools.h"

// Structs exist in both C++ and Verilog

int x = 1;

struct MyStruct1 {
  logic<8> a;
};

template<int x = 1>
class Fop {
public:
};

class Module {
public:

  Module() : foo(1), bar(2) {}

  MyStruct1 my_struct1;
  Fop<3> wejkr;

  logic<8> tock(int a, int b, int c) {
    my_struct1.a *= true;
    return a + --b + c++ * 7;
  }
};
