// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_MULTIPLEXER2_H
#define RVSIMPLE_MULTIPLEXER2_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

template <int WIDTH = 32>
class multiplexer2 {
 public:
  logic<WIDTH> out(logic<1> sel, logic<WIDTH> in0, logic<WIDTH> in1) {
    switch (sel) {
      case 0:  return in0;
      case 1:  return in1;
      default: return bx<WIDTH>(DONTCARE);
    }
  }
};

#endif  // RVSIMPLE_MULTIPLEXER2_H
