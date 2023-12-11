// A very basic counter in plain C++, converted to Verilog using Metron.

class Counter {
public:
  int count_;
  void update() {
    count_++;
  }
};
