#include "metron_tools.h"

//X Can't call non-function tick from function func.

class Module {
public:

  void tock() {
    func();
  }

  void func() const {
    tick();
  }

private:

  void tick() const {
  }

};
