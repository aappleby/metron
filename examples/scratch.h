struct Vec {
  int x;
  int y;
};

class Module {
public:

  Vec v;

  int tock() {
    return 2;
  }

  void tick() {
    v.x = tock();
    v.y += tock();
  }
};
