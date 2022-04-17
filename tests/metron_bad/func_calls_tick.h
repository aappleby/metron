#include "metron_tools.h"

//X Can't call non-function tick from function func.

class Module {
public:

  void tock() {
    func();
  }

private:

  void func() const {
    tick();
  }

  void tick() const {
  }

};
