
void derp() {}


struct Blah {

  void a() { b(); }
  void b() { c(); }
  void c() { d(); }
  void d() { e(); }
  void e() { f(); }
  void f() { f(); derp(); }

};

struct Blee {
  void derp() {
    Blah b;
    b.a();
  }
};
