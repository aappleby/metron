// RISC-V SiMPLE SV -- adder module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef ADDER_H
#define ADDER_H

#include "config.h"
#include "constants.h"
#include "metron/metron_tools.h"

template <int WIDTH = 32>
class adder {
 public:
  logic<WIDTH> operand_a;
  logic<WIDTH> operand_b;
  logic<WIDTH> result;

  void tock() { result = operand_a + operand_b; }
};

#endif // ADDER_H
