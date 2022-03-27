// RISC-V SiMPLE SV -- control transfer unit
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_CONTROL_TRANSFER_H
#define RVSIMPLE_CONTROL_TRANSFER_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class control_transfer {
 public:

  logic<1> take_branch(logic<1> result_equal_zero, logic<3> inst_funct3) const {
    using namespace rv_constants;
    switch (inst_funct3) {
      case FUNCT3_BRANCH_EQ:  return !result_equal_zero;
      case FUNCT3_BRANCH_NE:  return result_equal_zero;
      case FUNCT3_BRANCH_LT:  return !result_equal_zero;
      case FUNCT3_BRANCH_GE:  return result_equal_zero;
      case FUNCT3_BRANCH_LTU: return !result_equal_zero;
      case FUNCT3_BRANCH_GEU: return result_equal_zero;
      default:                return b1(DONTCARE);
    }
  }
};

#endif  // RVSIMPLE_CONTROL_TRANSFER_H
