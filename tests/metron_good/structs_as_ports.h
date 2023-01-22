#include "metron_tools.h"

// Structs can be used as input/output ports to submodules.

namespace TL {
  const int PutFullData = 0;
  const int PutPartialData = 1;
  const int Get = 4;
  const int AccessAck = 0;
  const int AccessAckData = 1;
};

struct tilelink_a {
  logic<3>  a_opcode;
  logic<32> a_address;
  logic<4>  a_mask;
  logic<32> a_data;
  logic<1>  a_valid;
};

struct tilelink_d {
  logic<3>  d_opcode;
  logic<32> d_data;
  logic<1>  d_valid;
};

//------------------------------------------------------------------------------

class TilelinkDevice {
public:

  TilelinkDevice() {
    test_reg = 0;
    oe = 0;
  }

  tilelink_d tld;
  tilelink_a tla;

  void tock() {
    if (oe) {
      tld.d_opcode = TL::AccessAckData;
      tld.d_data   = test_reg;
      tld.d_valid  = 1;
    }
    else {
      tld.d_opcode = TL::AccessAckData;
      tld.d_data   = b32(DONTCARE);
      tld.d_valid  = 0;
    }

    tick();
  }

private:
  void tick() {
    if (tla.a_address == 0x1234) {
      if (tla.a_opcode == TL::PutFullData && tla.a_valid) {
        test_reg = tla.a_data;
      } else if (tla.a_opcode == TL::Get) {
        oe = 1;
      }
    }
  }

  logic<32> test_reg;
  logic<1>  oe;
};

//------------------------------------------------------------------------------














#if 0

class TilelinkCPU {
public:

  tilelink_a tla;
  tilelink_d tld;

  TilelinkCPU() {
    addr = 0x1234;
  }

  void tock_tla() {
    tla.a_address = addr;
    tla.a_mask    = 0b1111;
    tla.a_data    = 0xDEADBEEF;
    tla.a_valid   = 1;
  }

  void tock_tld() {
  }

private:
  void tick() {
  }

  logic<32> addr;
  logic<32> reg;
};

//------------------------------------------------------------------------------

class Top {
public:

  void tock() {
    cpu.tock();
    dev.tock();

    cpu.tld = dev.tld;
    dev.tla = cpu.tla;

    cpu.tick();
    dev.tick();
  }

  TilelinkCPU cpu;
  TilelinkDevice dev;
};

//------------------------------------------------------------------------------
#endif
