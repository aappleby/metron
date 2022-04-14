#include "metron_tools.h"

// Getters must be const.

class Module {
public:

  logic<8> get_my_reg() {
    return my_reg;
  }

  void tick() {
    my_reg = my_reg + 1;
  }

private:
  logic<8> my_reg;
};
