#include "metron/metron_tools.h"

namespace MyPackage {
  static const int ADDER_WIDTH = 17;
  static const int INCREMENT_VAL = 3;
};

template<int width = 32>
class AdjustableAdder {
public:
  logic<width> add(logic<width> a, logic<width> b) {
    return a + b;
  }
};

class Module {
  public:
  logic<MyPackage::ADDER_WIDTH> increment(logic<MyPackage::ADDER_WIDTH> x) {
    return x + MyPackage::INCREMENT_VAL;
  }

private:
  AdjustableAdder<MyPackage::ADDER_WIDTH> my_adder;
};
