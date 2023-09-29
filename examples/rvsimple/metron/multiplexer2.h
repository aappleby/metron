// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef MULTIPLEXER2_H
#define MULTIPLEXER2_H

#include "config.h"
#include "constants.h"
#include "metron/metron_tools.h"

template <int WIDTH = 32>
class multiplexer2 {
 public:
  logic<WIDTH> in0;
  logic<WIDTH> in1;
  logic<1> sel;
  logic<WIDTH> out;

  void tock() {
    switch (sel) {
      case 0:
        out = in0;
        break;
      case 1:
        out = in1;
        break;
      default:
        out = DONTCARE;
        break;
    }
  }
};

#endif // MULTIPLEXER2_H
