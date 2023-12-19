#include "metron/metron_tools.h"

// Structs can be used as input/output ports to submodules.
// ...but they have to be public member variables because Yosys...

namespace TL {
  static const int PutFullData = 0;
  static const int PutPartialData = 1;
  static const int Get = 4;
  static const int AccessAck = 0;
  static const int AccessAckData = 1;
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
    test_reg_ = 0;
    oe_ = 0;
  }

  tilelink_d tld;
  tilelink_a tla;

  void tock() {
    if (oe_) {
      tld.d_opcode = TL::AccessAckData;
      tld.d_data   = test_reg_;
      tld.d_valid  = 1;
    }
    else {
      tld.d_opcode = TL::AccessAckData;
      tld.d_data   = DONTCARE;
      tld.d_valid  = 0;
    }
  }

  void tick() {
    if (tla.a_address == 0x1234) {
      if (tla.a_opcode == TL::PutFullData && tla.a_valid) {
        logic<32> mask = cat(
          dup<8>(tla.a_mask[0]),
          dup<8>(tla.a_mask[1]),
          dup<8>(tla.a_mask[2]),
          dup<8>(tla.a_mask[3]));
        test_reg_ = (test_reg_ & ~mask) | (tla.a_data & mask);
        oe_ = 0;
      } else if (tla.a_opcode == TL::Get) {
        oe_ = 1;
      }
    }
  }

private:
  logic<32> test_reg_;
  logic<1>  oe_;
};

//------------------------------------------------------------------------------

class TilelinkCPU {
public:

  tilelink_a tla;
  tilelink_d tld;

  TilelinkCPU() {
    addr_ = 0x1234;
    data_ = 0x4321;
  }

  void tock() {
    if (data_ & 1) {
      tla.a_opcode  = TL::Get;
      tla.a_address = addr_;
      tla.a_mask    = 0b1111;
      tla.a_data    = DONTCARE;
      tla.a_valid   = 1;
    } else {
      tla.a_opcode  = TL::PutFullData;
      tla.a_address = addr_;
      tla.a_mask    = 0b1111;
      tla.a_data    = 0xDEADBEEF;
      tla.a_valid   = 1;
    }
  }

  void tick() {
    if (tld.d_opcode == TL::AccessAckData && tld.d_valid) {
      data_ = tld.d_data;
    }
  }

private:
  logic<32> addr_;
  logic<32> data_;
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
