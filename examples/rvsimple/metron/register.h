// RISC-V SiMPLE SV -- generic register
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_REGISTER_H
#define RVSIMPLE_REGISTER_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

template <int WIDTH = 32, int INITIAL = 0>
class single_register {
 public:
  single_register() { value = INITIAL; }
  logic<WIDTH> value;

  void tock(logic<1> reset, logic<1> write_enable, logic<WIDTH> next) {
    tick(reset, write_enable, next);
  }

 private:
  void tick(logic<1> reset, logic<1> write_enable, logic<WIDTH> next) {
    if (reset)
      value = INITIAL;
    else if (write_enable)
      value = next;
  }
};

#endif  // RVSIMPLE_REGISTER_H
