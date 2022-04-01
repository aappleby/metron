// RISC-V SiMPLE SV -- immediate generator
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_IMMEDIATE_GENERATOR_H
#define RVSIMPLE_IMMEDIATE_GENERATOR_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class immediate_generator {
 public:

  // Immediate format
  //       31.............30........20.19........12.11.....11.10.........5.4..........1.0.....0
  // I = { {21{inst[31]}},                                     inst[30:25], inst[24:20]         };
  // S = { {21{inst[31]}},                                     inst[30:25], inst[11:7]          };
  // B = { {20{inst[31]}}, inst[7],                            inst[30:25], inst[11:8],   1'b0  };
  // U = { {1{inst[31]}},  inst[30:20], inst[19:12],                                      12'b0 };
  // J = { {12{inst[31]}},              inst[19:12], inst[20], inst[30:25], inst[24:21],  1'b0  };

  logic<32> immediate(logic<32> inst) const {
    using namespace rv_constants;
    switch (b7(inst)) { // Opcode
      // FIXME we need to translate fallthrough into "x, y, z:"?
      case OPCODE_LOAD: // I-type immediate
        return cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20)); break;
      case OPCODE_LOAD_FP: // I-type immediate
        return cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20)); break;
      case OPCODE_OP_IMM: // I-type immediate
        return cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20)); break;
      case OPCODE_JALR: // I-type immediate
        return cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20)); break;
      case OPCODE_STORE_FP: // S-type immediate
        return cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 7)); break;
      case OPCODE_STORE: // S-type immediate
        return cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 7)); break;
      case OPCODE_BRANCH: // B-type immediate
        return cat(dup<20>(inst[31]), inst[7], b6(inst, 25), b4(inst, 8), b1(0)); break;
      case OPCODE_AUIPC: // U-type immediate
        return cat(inst[31], b11(inst, 20), b8(inst, 12), b12(0)); break;
      case OPCODE_LUI: // U-type immediate
        return cat(inst[31], b11(inst, 20), b8(inst, 12), b12(0)); break;
      case OPCODE_JAL: // J-type immediate
        return cat(dup<12>(inst[31]), b8(inst, 12), inst[20], b6(inst, 25), b4(inst, 21), b1(0)); break;
      default:
        return b32(0);
    }
  }
};

#endif  // RVSIMPLE_IMMEDIATE_GENERATOR_H
