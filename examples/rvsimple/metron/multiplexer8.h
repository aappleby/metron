// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef MULTIPLEXER8_H
#define MULTIPLEXER8_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

template <int WIDTH = 32>
class multiplexer8 {
 public:
  logic<WIDTH> in0;
  logic<WIDTH> in1;
  logic<WIDTH> in2;
  logic<WIDTH> in3;
  logic<WIDTH> in4;
  logic<WIDTH> in5;
  logic<WIDTH> in6;
  logic<WIDTH> in7;
  logic<3> sel;
  logic<WIDTH> out;

  void tock() {
    // clang-format off
    switch (sel) {
      case 0:  out = in0; break;
      case 1:  out = in1; break;
      case 2:  out = in2; break;
      case 3:  out = in3; break;
      case 4:  out = in4; break;
      case 5:  out = in5; break;
      case 6:  out = in6; break;
      case 7:  out = in7; break;
      default: out = DONTCARE; break;
    }
    // clang-format on
  }
};

#endif // MULTIPLEXER8_H
