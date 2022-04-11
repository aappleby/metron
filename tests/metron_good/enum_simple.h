#include "metron_tools.h"

// bad
// enum { FOO, BAR, BAZ };
// typedef enum logic[1:0] { FOO=70, BAR=71, BAZ=72 } blem;
// typedef enum { FOO, BAR=0, BAZ=1 } blem;

// good
// enum { FOO, BAR, BAZ } blem;
// enum { FOO=0, BAR=1, BAZ=2 } blem;
// typedef enum { FOO, BAR, BAZ } blem;
// typedef enum { FOO=0, BAR=1, BAZ=2 } blem;
// typedef enum logic[1:0] { FOO, BAR, BAZ } blem;
// typedef enum logic[1:0] { FOO=0, BAR=1, BAZ=2 } blem;


// Simple anonymous enums should work.
// FIXME DIS DOESNT WORK

class Module {
public:

  enum blem { FOO, BAR, BAZ };

  void tock() {
    logic<2> x = FOO;
    blem y = BAR;

    if (x == FOO) {
      y = BAZ;
    }
  }

};
