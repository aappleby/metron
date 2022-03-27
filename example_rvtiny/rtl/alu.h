// RISC-V SiMPLE SV -- ALU module
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_ALU_H
#define RVSIMPLE_ALU_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class alu {
 public:

  logic<32> result(logic<5> alu_function, logic<32> operand_a, logic<32> operand_b) {
    using namespace rv_constants;

    switch (alu_function) {
      case ALU_ADD:  return operand_a + operand_b;
      case ALU_SUB:  return operand_a - operand_b;
      case ALU_SLL:  return operand_a << b5(operand_b);
      case ALU_SRL:  return operand_a >> b5(operand_b);
      case ALU_SRA:  return sra(operand_a, b5(operand_b));
      case ALU_SEQ:  return cat(b31(0), b1(operand_a == operand_b));
      case ALU_SLT:  return cat(b31(0), b1(signed(operand_a) < signed(operand_b)));
      case ALU_SLTU: return cat(b31(0), b1(unsigned(operand_a) < unsigned(operand_b)));
      case ALU_XOR:  return operand_a ^ operand_b;
      case ALU_OR:   return operand_a | operand_b;
      case ALU_AND:  return operand_a & operand_b;
      default:       return b32(ZERO);
    }
  }
};

#endif  // RVSIMPLE_ALU_H
