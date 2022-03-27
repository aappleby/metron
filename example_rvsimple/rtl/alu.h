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
  logic<32> result;
  logic<1> result_equal_zero() const {
    return (result == b32(0));
  }

#ifdef M_MODULE
 private:
  logic<64> signed_multiplication;
  logic<64> unsigned_multiplication;
  logic<64> signed_unsigned_multiplication;

 public:
#endif

  void tock(logic<5> alu_function, logic<32> operand_a, logic<32> operand_b) {
    using namespace rv_constants;

    switch (alu_function) {
      case ALU_ADD:
        result = operand_a + operand_b;
        break;
      case ALU_SUB:
        result = operand_a - operand_b;
        break;
      case ALU_SLL:
        result = operand_a << b5(operand_b);
        break;
      case ALU_SRL:
        result = operand_a >> b5(operand_b);
        break;
      case ALU_SRA:
        result = sra(operand_a, b5(operand_b));
        break;
      case ALU_SEQ:
        result = cat(b31(0), b1(operand_a == operand_b));
        break;
      case ALU_SLT:
        result = cat(b31(0), b1(signed(operand_a) < signed(operand_b)));
        break;
      case ALU_SLTU:
        result = cat(b31(0), b1(unsigned(operand_a) < unsigned(operand_b)));
        break;
      case ALU_XOR:
        result = operand_a ^ operand_b;
        break;
      case ALU_OR:
        result = operand_a | operand_b;
        break;
      case ALU_AND:
        result = operand_a & operand_b;
        break;

#ifdef M_MODULE
      case ALU_MUL:
        result = b32(signed_multiplication, 0);
        break;
      case ALU_MULH:
        result = b32(signed_multiplication, 32);
        break;
      case ALU_MULHSU:
        result = b32(signed_unsigned_multiplication, 32);
        break;
      case ALU_MULHU:
        result = b32(unsigned_multiplication, 32);
        break;
      case ALU_DIV:
        if (operand_b == ZERO)
          result = 1;
        else if ((operand_a == 0x80000000) && (operand_b == 1))
          result = 0x80000000;
        else
          result = operand_a / operand_b;
        break;
      case ALU_DIVU:
        if (operand_b == ZERO)
          result = 1;
        else
          result = unsigned(operand_a) / unsigned(operand_b);
        break;
      case ALU_REM:
        if (operand_b == ZERO)
          result = operand_a;
        else if ((operand_a == 0x80000000) && (operand_b == 1))
          result = ZERO;
        else
          result = operand_a % operand_b;
        break;
      case ALU_REMU:
        if (operand_b == ZERO)
          result = operand_a;
        else
          result = unsigned(operand_a) % unsigned(operand_b);
        break;
#endif

      default:
        result = b32(ZERO);
        break;
    }
  }

#ifdef M_MODULE
  void tock_mult() {
    signed_multiplication = operand_a * operand_b;
    unsigned_multiplication = unsigned(operand_a) * unsigned(operand_b);
    signed_unsigned_multiplication = signed(operand_a) * unsigned(operand_b);
  }
#endif
};

#endif  // RVSIMPLE_ALU_H
