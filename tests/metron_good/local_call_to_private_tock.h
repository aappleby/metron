#include "metron_tools.h"

class Module {
public:

  logic<8> my_sig;

  logic<8> tock1(logic<8> a) {
    tock2(a + 4);
    return tock3(a + 7);
  }

private:

  void tock2(logic<8> a) {
    my_sig = a + 22;
  }

  logic<8> tock3(logic<8> a) {
    return a + 13;
  }


};
