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
  int sum;

  void add() {
    sum = a + b;
  }
};
