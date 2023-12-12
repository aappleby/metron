#include "metron/metron_tools.h"

// Structs exist in both C++ and Verilog

struct MyStruct1 {
  logic<8> a;
};

class Module {
public:

  MyStruct1 my_struct1;

  MyStruct1 get_something() {
    return my_struct1;
  }

  logic<8> tock_ret;
  void tock() {
    my_struct1.a = 1;

    // FIXME does not work in yosys
    //return extract_field(my_struct1);
    tock_ret = 16;
  }

private:

  // FIXME does not work in yosys
  /*
  logic<8> extract_field(MyStruct1 m) {
    return m.a;
  }
  */

};
