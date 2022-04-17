#include "metron_tools.h"

// Getter methods should turn into outputs.

class Module {
public:

  logic<7> tock_get_reg() const {
    return my_reg;
  }

  void tock() {
    tick();
  }


private:

  void tick() {
    my_reg = my_reg + 1;
  }

  logic<7> my_reg;
};
