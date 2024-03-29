#include "metron/metron_tools.h"

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
  void tock_no_params() {
    my_sig1 = 12;
    int x = my_sig1;
  }

  int my_sig3;
  void tock_params(int x) {
    my_sig3 = 12 + x;
    int y = my_sig3;
  }

  int my_sig5a;
  void tock_calls_funcs1(int x) {
    my_sig5a = 12 + my_func5(x);
  }

  int my_sig5b;
  void tock_calls_funcs2(int x) {
    my_sig5b = 2 + my_func5(x - 7);
  }

private:
  int my_func5(int x) {
    return x + 1;
  }
public:

  int my_sig6a;
  void tock_calls_tock(int x) {
    my_sig6a = 12;
    tock_called_by_tock(my_sig6a);
  }

private:
  int my_sig6b;
  void tock_called_by_tock(int x) {
    my_sig6b = x;
  }
public:

  //----------

  int my_reg1_;
  void tick_no_params() {
    my_reg1_ = my_reg1_ + 1;
    tick_called_by_tick(func_called_by_tick(1));
  }

  int my_reg2_;
  void tick_params(int x) {
    my_reg2_ = my_reg2_ + x;
  }

private:
  int my_reg3_;
  void tick_called_by_tick(int x) {
    my_reg3_ = my_reg3_ + x;
  }

  int func_called_by_tick(int x) {
    return x + 7;
  }

public:

  int my_sig6c;
  void tock_calls_private_tick() {
    my_sig6c = 17;
    tick_private(my_sig6c);
  }

private:
  int my_reg4_;
  void tick_private(int x) {
    my_reg4_ = my_reg4_ + x;
  }


};
