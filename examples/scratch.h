class Module {
public:

  int x;
  int y;

  void tock() {
    y = x + 1;
    x = y + 1;
  }
};
