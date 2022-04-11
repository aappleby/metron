#include "metron_tools.h"

// Port and function arg names can collide, the latter is disambiguated by its
// function name.

class Module {
public:

  logic<3> input_val;
  logic<3> output_val;

  void tock1() {
    output_val = input_val + 7;
  }

  logic<3> tock(logic<3> input_val) {
    return input_val + 8;
  }
};
