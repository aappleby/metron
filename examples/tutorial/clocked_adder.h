// FIXME this example is outdated

// By default, "sum" is an output signal assigned in always_comb.
class Adder1 {
public:
  int sum;
  void add(int a, int b) {
    sum = a + b;
  }
};

// We can force Metron to emit "sum" as a register by reading from it before
// we write to it.
class Adder2 {
public:
  int sum_;
  void add(int a, int b) {
    int dummy = sum_;
    sum_ = a + b;
  }
};

// Or we can prefix the function name with "tick", which does basically the
// same thing.
class Adder3 {
public:
  int sum_;
  void tick_add(int a, int b) {
    sum_ = a + b;
  }
};
