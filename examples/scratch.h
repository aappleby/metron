// Prefixing a method with "tick_" should force it to be a tick if it would
// otherwise be ambiguous

class Adder {
public:

  int tick_add(int a, int b) {
    return a + b;
  }
};
