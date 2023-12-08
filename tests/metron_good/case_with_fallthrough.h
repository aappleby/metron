#include "metron/metron_tools.h"

// Case statements are allowed to have fallthrough, though the SV syntax is
// a bit different.

class Module {
public:

  logic<8> tock() {
    logic<8> result;
    switch(my_reg_) {
      case 0: // can we stick comments in here?
      case 1:
      case 2:
        result = 10;
        break;
      case 3: {
        result = 20;
        break;
      }
      default:
        result = 30;
        break;
    }

    tick();
    return result;
  }

private:

  void tick() {
    my_reg_ = my_reg_ + 1;
  }

  logic<8> my_reg_;
};
