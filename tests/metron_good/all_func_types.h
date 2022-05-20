#include "metron_tools.h"

class Module {
public:

  int my_sig1;
  int my_sig2;
  int my_sig3;
  int my_sig4;
  int my_reg1;
  int my_reg2;

  /*
  void func_no_params_no_return() {
  }
  */

  int func_no_params_return() {
    return 1;
  }

  /*
  void func_params_no_return(int x) {
  }
  */

  int func_params_return(int x) {
    return x + 1;
  }

  void tock_no_params_no_return() {
    my_sig1 = 12;
    int x = my_sig1;
  }

  int tock_no_params_return() {
    my_sig2 = 12;
    return my_sig2;
  }

  void tock_params_no_return(int x) {
    my_sig3 = 12 + x;
    int y = my_sig3;
  }

  int tock_params_return(int x) {
    my_sig4 = 12 + x;
    return my_sig4;
  }

  void tick_no_params() {
    my_reg1 = my_reg1 + 1;
  }

  void tick_params(int x) {
    my_reg2 = my_reg2 + x;
  }
};
