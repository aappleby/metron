#include "metron_tools.h"

//X Init method Module::blahblah is not allowed to have params

class Module {
public:

  Module() {
    blahblah(1);
  }

private:

  void blahblah(int x) {
  }
};
