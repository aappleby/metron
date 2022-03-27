// RISC-V SiMPLE SV -- multiplexer module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_MULTIPLEXER8_H
#define RVSIMPLE_MULTIPLEXER8_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

template <int WIDTH = 32>
class multiplexer8 {
 public:

  logic<WIDTH> out(logic<3> sel, logic<WIDTH> in0, logic<WIDTH> in1, logic<WIDTH> in2,
            logic<WIDTH> in3, logic<WIDTH> in4, logic<WIDTH> in5,
            logic<WIDTH> in6, logic<WIDTH> in7) const {
    switch (sel) {
      case 0:  return in0;
      case 1:  return in1;
      case 2:  return in2;
      case 3:  return in3;
      case 4:  return in3;
      case 5:  return in3;
      case 6:  return in3;
      case 7:  return in3;
      default: return bx<WIDTH>(DONTCARE);
    }
  }
};

#endif  // RVSIMPLE_MULTIPLEXER2_H
