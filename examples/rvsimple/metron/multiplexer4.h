// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_MULTIPLEXER4_H
#define RVSIMPLE_MULTIPLEXER4_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

template <int WIDTH = 32>
class multiplexer4 {
 public:

  logic<WIDTH> out(logic<2> sel, logic<WIDTH> in0, logic<WIDTH> in1, logic<WIDTH> in2, logic<WIDTH> in3) {
    switch (sel) {
      case 0:  return in0;
      case 1:  return in1;
      case 2:  return in2;
      case 3:  return in3;
      default: return bx<WIDTH>(DONTCARE);
    }
  }
};

#endif  // RVSIMPLE_MULTIPLEXER4_H
