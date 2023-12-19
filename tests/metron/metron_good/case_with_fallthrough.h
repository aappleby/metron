#include "metron/metron_tools.h"

// Case statements are allowed to have fallthrough, though the SV syntax is
// a bit different.

class Module {
public:

  logic<8> tock_result;
  void tock() {
    switch(my_reg_) {
      case 0: // can we stick comments in here?
      case 1:
      case 2:
        tock_result = 10;
        break;
      case 3: {
        tock_result = 20;
        break;
      }
      default:
        tock_result = 30;
        break;
    }

    tick(tock_result);
  }

private:

  void tick(logic<8> inc) {
    my_reg_ = my_reg_ + inc;
  }

  logic<8> my_reg_;
};
