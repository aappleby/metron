#include "metron_tools.h"

class Module {
public:


  /*
  // but why would you do this?
  void func_no_params_no_return() {
  }

  // or this?
  void func_params_no_return(int x) {
  }
  */

  int func_no_params_return() {
    return 1;
  }

  int func_params_return(int x) {
    return x + 1;
  }

  int my_sig1;
  void tock_no_params_no_return() {
    my_sig1 = 12;
    int x = my_sig1;
  }

  int my_sig2;
  int tock_no_params_return() {
    my_sig2 = 12;
    return my_sig2;
  }

  int my_sig3;
  void tock_params_no_return(int x) {
    my_sig3 = 12 + x;
    int y = my_sig3;
  }

  int my_sig4;
  int tock_params_return(int x) {
    my_sig4 = 12 + x;
    return my_sig4;
  }

  int my_sig5;
  void tock_calls_funcs(int x) {
    my_sig5 = 12 + my_func5(x);
  }

  int my_func5(int x) { return x + 1; }

  int my_sig6a;
  int tock_calls_tock(int x) {
    my_sig6a = 12;
    tock_called_by_tock(my_sig6a);
    return 0;
  }

  int my_sig6b;
  void tock_called_by_tock(int x) {
    my_sig6b = x;
  }

  //----------

  int my_reg1;
  int my_reg2;

  void tick_no_params() {
    my_reg1 = my_reg1 + 1;
  }

  void tick_params(int x) {
    my_reg2 = my_reg2 + x;
  }
};
