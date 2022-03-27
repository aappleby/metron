// RISC-V SiMPLE SV -- instruction decoder
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_INSTRUCTION_DECODER_H
#define RVSIMPLE_INSTRUCTION_DECODER_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class instruction_decoder {
 public:
  logic<7> inst_opcode2(logic<32> inst) const { return b7(inst, 0); }
  logic<3> inst_funct32(logic<32> inst) const { return b3(inst, 12); }
  logic<7> inst_funct72(logic<32> inst) const { return b7(inst, 25); }
  logic<5> inst_rd2(logic<32> inst)     const { return b5(inst, 7); }
  logic<5> inst_rs12(logic<32> inst)    const { return b5(inst, 15); }
  logic<5> inst_rs22(logic<32> inst)    const { return b5(inst, 20); }
};

#endif  // RVSIMPLE_INSTRUCTION_DECODER_H
