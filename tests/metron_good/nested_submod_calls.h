#include "metron_tools.h"

// Nesting submod calls should put all the nested input bindings above the call.

class Submod1 {
public:
  logic<8> add_one(logic<8> a) const {
    return a + 1;
  }
};

class Submod2 {
public:
  logic<8> add_two(logic<8> a) const {
    return a + 1;
  }
};

class Module {
public:

  void tick() {
    // Two bindings should end up here.
    counter = submod1.add_one(submod2.add_two(counter));
  }

private:

  Submod1 submod1;
  Submod2 submod2;
  logic<8> counter;
};
