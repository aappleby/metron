#include "metron/metron_tools.h"

// Getter methods should turn into outputs.

class Module {
public:

  logic<7> get_reg() const {
    return my_reg_;
  }

  void tock() {
    tick();
  }


private:

  void tick() {
    my_reg_ = my_reg_ + 1;
  }

  logic<7> my_reg_;
};
