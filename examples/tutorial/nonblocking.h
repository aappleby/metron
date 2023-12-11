class NonBlockingAssignment {
public:
  int count_;
  void update() {
    count_ = count_ + 1;
    int dummy = count_;
  }
};
