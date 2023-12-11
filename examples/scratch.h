#include "metron/metron_tools.h"

class Module {
 public:

  void tick(logic<16> addr) {
    switch (addr) {

      // metron didn't like the block without {}
      case 0xFF1C: {
        switch (1) {
          case 0: break;
        }
        break;
      }
    }
  }

  logic<8> data_out_;
};
