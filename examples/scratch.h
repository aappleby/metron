class Module {
public:

  int x;
  int y;

  void tock(int c) {
    y = x + c;
    x = y + c;
  }

  void tack(int b) {
    tock(b + 1);
  }

  void tick(int a) {
    tack(a + 1);
  }
};
