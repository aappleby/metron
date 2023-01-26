#include "metron_tools.h"

/* verilator lint_off UNUSEDSIGNAL */
// verilator lint_off undriven

struct tilelink_d {
  logic<3>  d_opcode;
};


class pinwheel_core {
public:
  void tock(tilelink_d bus_tld) {
  }
};

class pinwheel {
public:

  pinwheel_core core;
  tilelink_d bus_tld;
  tilelink_d bus_tld2;
  logic<1> cond_1;

  void tock() {
    bus_tld.d_opcode = b3(DONTCARE);
    if (cond_1 == 1)  bus_tld = bus_tld2;
    core.tock(bus_tld);
  }

  void tick() {
    bus_tld2.d_opcode = bus_tld2.d_opcode + 1;
  }
};
