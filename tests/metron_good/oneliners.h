#include "metron_tools.h"

// Cramming various statements into one line should not break anything.

class Module {
public:

  logic<8> tock() { logic<8> a = 1; a = a + 7; return a; }
  void tick() { if (my_reg & 1) my_reg = my_reg - 7; }

private:
  logic<8> my_reg;

};
