#include "metron/metron_tools.h"

class DPRam {
public:

  logic<16> porta_dout;
  logic<16> portb_dout;

  void tick(
    logic<16> porta_addr,
    logic<16> portb_addr,
    logic<1> portb_write,
    logic<16> portb_data) {

    porta_dout = ram[porta_addr];

    if (write) {
      ram[portb_addr] = portb_data;
      portb_dout = portb_data;
    }
    else {
      portb_dout = ram[portb_addr];
    }
  }

private:
  logic<16> ram[65536];
};
