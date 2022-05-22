struct InnerStruct {
  int a;
  int b;
  int c;
};

struct OuterStruct {
  InnerStruct x;
  InnerStruct y;
  InnerStruct z;
};

class Module {
public:

  void tock() {
    s.x.a = 1;
    s.x.b = 2;
    s.x.c = 3;

    s.y.a = 4;
    s.y.b = 5;
    s.y.c = 6;

    s.z.a = 7;
    s.z.b = 8;
    s.z.c = 9;
  }

  OuterStruct s;
};
