// If "sum" does not have an underscore suffix, it is a signal and "add" will
// be converted to an always_comb block.
class Adder1 {
public:
  int sum;
  void add(int a, int b) {
    sum = a + b;
  }
};

// If "sum" does have an underscore suffix, it is a register and "add" will
// be converted to an always_ff block.
class Adder2 {
public:
  int sum_;
  void add(int a, int b) {
    sum_ = a + b;
  }
};
