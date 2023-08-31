class Module {
public:

  int x;

  int tock() {
    return 2;
  }

  void tick() {
    x = tock();
  }
};
