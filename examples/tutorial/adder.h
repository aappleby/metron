class Adder {
public:
  int add(int a, int b) {
    return a + b;
  }
};

//----------

class Adder2 {
public:
  int a;
  int b;
  int sum_;

  void add() {
    sum_ = a + b;
  }
};
