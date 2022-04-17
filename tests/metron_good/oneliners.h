#include "metron_tools.h"

// Cramming various statements into one line should not break anything.

class Module {
public:

  void tock1() { tick(); }
  logic<8> tock2() { logic<8> a = 1; a = a + 7; return a; }

private:

  void tick() { if (my_reg & 1) my_reg = my_reg - 7; }

  logic<8> my_reg;

};
