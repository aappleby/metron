// Copyright lowRISC contributors.
// Copyright 2018 ETH Zurich and University of Bologna, see also CREDITS.md.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "metron_tools.h"
#include "ibex_pkg.h"

/**
 * Arithmetic logic unit
 */
template<ibex_pkg::rv32b_e RV32B> // scope getting cut off enum
class ibex_alu {
public:

private:
  logic<32> operand_a_rev;
  logic<33> operand_b_neg;
public:

  void tock_rev(logic<32> operand_a_i) {
    for (int k = 0; k < 32; k++) {
      operand_a_rev[k] = operand_a_i[31-k];
    }
  }

  ///////////
  // Adder //
  ///////////

private:
  logic<1>  adder_op_a_shift1;
  logic<1>  adder_op_a_shift2;
  logic<1>  adder_op_a_shift3;
  logic<1>  adder_op_b_negate;
  //logic<33> adder_in_a, adder_in_b;
  //logic<32> adder_result;
public:

  void tock1(ibex_pkg::alu_op_e operator_i) {
    using namespace ibex_pkg;

    adder_op_a_shift1 = 0;
    adder_op_a_shift2 = 0;
    adder_op_a_shift3 = 0;
    adder_op_b_negate = 0;

    /*#unique#*/ switch(operator_i) {
      // Adder OPs
      case alu_op_e::ALU_SUB:
        adder_op_b_negate = 1;
        break;

      // Comparator OPs
      case alu_op_e::ALU_EQ:
      case alu_op_e::ALU_NE:
      case alu_op_e::ALU_GE:
      case alu_op_e::ALU_GEU:
      case alu_op_e::ALU_LT:
      case alu_op_e::ALU_LTU:
      case alu_op_e::ALU_SLT:
      case alu_op_e::ALU_SLTU:
        adder_op_b_negate = 1;
        break;

      default:
        break;
    }
  }


  void tick() {
    operand_b_neg = 0;
  }


};
