#include "metron/metron_tools.h"

// Structs exist in both C++ and Verilog

struct MyStruct1 {
  logic<8> a;
  logic<8> b;
};

union MyUnion1 {
  logic<8> a;
  logic<8> b;
};

class Module {
public:

  void tick() {
    my_struct_.a = my_struct_.b + 1;
    my_union_.a = my_union_.b + 1;
  }

private:

  MyStruct1 my_struct_;
  MyUnion1 my_union_;

};
