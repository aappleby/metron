#include "metron_tools.h"

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
