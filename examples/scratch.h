#include "metron_tools.h"

class Module {
public:

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
};
