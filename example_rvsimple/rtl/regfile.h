// RISC-V SiMPLE SV -- register file
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_REGFILE_H
#define RVSIMPLE_REGFILE_H

#include "config.h"
#include "constants.h"
#include "metron_tools.h"

class regfile {
 public:
  // Register x0 is always 0
  void init() { regs[0] = b32(0); }

  logic<32> rs1_data(logic<5> rs1_address) const {
    return regs[rs1_address];
  }

  logic<32> rs2_data(logic<5> rs2_address) const {
    return regs[rs2_address];
  }

  // Write port for rd
  void tick(logic<1> write_enable, logic<5> rd_address, logic<32> rd_data) {
    if (write_enable && rd_address != b5(0)) {
      regs[rd_address] = rd_data;
    }
  }

 private:
  // 32 registers of 32-bit width
  logic<32> regs[32];
};

#endif  // RVSIMPLE_REGFILE_H
