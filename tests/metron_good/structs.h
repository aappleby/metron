#include "metron_tools.h"

// Structs exist in both C++ and Verilog

typedef struct {
  logic<8> a;
  int b;
  logic<2> c;
} MyStruct;

class Module {
public:
  logic<8> tock() {
    MyStruct my_struct;
    my_struct.a = b8(1234);
    my_struct.b = 12;
    my_struct.c = 0b01;
    return 17;
  }
};
