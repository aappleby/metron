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

  logic<WIDTH> out(logic<2> sel, logic<WIDTH> in0, logic<WIDTH> in1, logic<WIDTH> in2, logic<WIDTH> in3) const {
    logic<WIDTH> result;
    switch (sel) {
      case 0:  result = in0; break;
      case 1:  result = in1; break;
      case 2:  result = in2; break;
      case 3:  result = in3; break;
      default: result = bx<WIDTH>(DONTCARE); break;
    }
    return result;
  }
};

#endif  // RVSIMPLE_MULTIPLEXER4_H
