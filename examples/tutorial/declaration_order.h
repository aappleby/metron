// FIXME this example is outdated

// If we try to update mutually-dependent fields in one function, we'll hit an
// error. Uncomment to see the error.

/*
class Module1 {
public:
  int a;
  int b;

  void update() {
    a = b + 1;
    b = a + 1;
  }
};
*/

// This module declares "update_a" first.
class Module2 {
public:
  int a_;
  int b;

  void update_a() {
    a_ = b + 1;
  }

  void update_b() {
    b = a_ + 1;
  }
};

// This module declares "update_b" first.
class Module3 {
public:
  int a;
  int b_;

  void update_a() {
    a = b_ + 1;
  }

  void update_b() {
    b_ = a + 1;
  }
};

// This module uses temporaries to update both A and B in a single function
class Module4 {
public:
  int a_;
  int b_;

  void update() {
    int old_a = a_;
    int old_b = b_;
    a_ = old_b + 1;
    b_ = old_a + 1;
  }
};
