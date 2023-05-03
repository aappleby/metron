// RISC-V SiMPLE SV -- generic register
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef REGISTER_H
#define REGISTER_H

#include "config.h"
#include "constants.h"
#include "metron/tools/metron_tools.h"

template <int WIDTH = 32, int INITIAL = 0>
class single_register {
 public:
  logic<1> reset;
  logic<1> write_enable;
  logic<WIDTH> next;
  logic<WIDTH> value;

  single_register() { value = INITIAL; }

  void tock() { tick(); }

 private:
  void tick() {
    if (reset)
      value = INITIAL;
    else if (write_enable)
      value = next;
  }
};

#endif // REGISTER_H
