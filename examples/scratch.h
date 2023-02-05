#pragma once
#include "metron_tools.h"

class Pong {
 public:

  void tock_video() {
    logic<1> vga_R = 1;
    if (1) {
      vga_R = in_border();
    }
  }

  logic<1> in_border() const {
    return 0;
  }
};
