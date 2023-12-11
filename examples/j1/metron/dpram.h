#include "metron/metron_tools.h"

class DPRam {
public:

  logic<16> porta_dout_;
  logic<16> portb_dout_;

  void tick(
    logic<16> porta_addr,
    logic<16> portb_addr,
    logic<1>  portb_write,
    logic<16> portb_data) {

    porta_dout_ = ram_[porta_addr];

    if (portb_write) {
      ram_[portb_addr] = portb_data;
      portb_dout_ = portb_data;
    }
    else {
      portb_dout_ = ram_[portb_addr];
    }
  }

private:
  logic<16> ram_[65536];
};
