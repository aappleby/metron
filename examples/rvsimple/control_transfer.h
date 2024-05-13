// RISC-V SiMPLE SV -- control transfer unit
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef CONTROL_TRANSFER_H
#define CONTROL_TRANSFER_H

#include "config.h"
#include "constants.h"
#include "metron/metron_tools.h"

class control_transfer {
 public:
  logic<1> result_equal_zero;
  logic<3> inst_funct3;
  logic<1> take_branch;

  void tock_take_branch() {
    using namespace rv_constants;
    // clang-format off
    switch (inst_funct3) {
      case FUNCT3_BRANCH_EQ:  take_branch = !result_equal_zero; break;
      case FUNCT3_BRANCH_NE:  take_branch = result_equal_zero; break;
      case FUNCT3_BRANCH_LT:  take_branch = !result_equal_zero; break;
      case FUNCT3_BRANCH_GE:  take_branch = result_equal_zero; break;
      case FUNCT3_BRANCH_LTU: take_branch = !result_equal_zero; break;
      case FUNCT3_BRANCH_GEU: take_branch = result_equal_zero; break;
      default:                take_branch = DONTCARE; break;
    }
    // clang-format on
  }
};

#endif // CONTROL_TRANSFER_H
