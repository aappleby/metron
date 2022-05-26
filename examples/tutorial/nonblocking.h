class NonBlockingAssignment {
public:
  int count;
  void update() {
    count = count + 1;
    int dummy = count;
  }
};
