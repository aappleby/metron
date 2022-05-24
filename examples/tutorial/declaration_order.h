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
  int a;
  int b;

  void update_a() {
    a = b + 1;
  }

  void update_b() {
    b = a + 1;
  }
};

// This module declares "update_b" first.
class Module3 {
public:
  int a;
  int b;

  void update_b() {
    b = a + 1;
  }

  void update_a() {
    a = b + 1;
  }
};

// This module uses temporaries to update both A and B in a single function
class Module4 {
public:
  int a;
  int b;

  void update() {
    int old_a = a;
    int old_b = b;
    a = old_b + 1;
    b = old_a + 1;
  }
};
