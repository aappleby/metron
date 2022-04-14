#include "metron_tools.h"

// Structs exist in both C++ and Verilog

typedef struct {
  logic<8> a;
  int b;
  logic<2> c;
} MyStruct1;

struct MyStruct2 {
  logic<8> a;
  int b;
  logic<2> c;
};

class Module {
public:
  logic<8> tock() {
    MyStruct1 my_struct1;
    my_struct1.a = b8(1234);
    my_struct1.b = 12;
    my_struct1.c = 0b01;

    MyStruct2 my_struct2;
    my_struct2.a = b8(1234);
    my_struct2.b = 12;
    my_struct2.c = 0b01;

    return 17;
  }
};
