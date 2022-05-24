// We can force Metron to emit "result" as a register by reading from it before
// we write to it.
class Adder1 {
public:
  int result;
  void add(int a, int b) {
    int dummy = result;
    result = a + b;
  }
};

// Or we can prefix the function name with "tick", which does basically the
// same thing.
class Adder2 {
public:
  int tick_add(int a, int b) {
    return a + b;
  }
};
