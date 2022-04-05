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
  logic<WIDTH> out(logic<1> sel, logic<WIDTH> in0, logic<WIDTH> in1) const {
    logic<WIDTH> result;
    switch (sel) {
      case 0:  result = in0; break;
      case 1:  result = in1; break;
      default: result = bx<WIDTH>(DONTCARE); break;
    }
    return result;
  }
};

#endif  // RVSIMPLE_MULTIPLEXER2_H
