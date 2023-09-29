// RISC-V SiMPLE SV -- register file
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef REGFILE_H
#define REGFILE_H

#include "config.h"
#include "constants.h"
#include "metron/metron_tools.h"

class regfile {
 public:
  logic<1> write_enable;
  logic<5> rd_address;
  logic<5> rs1_address;
  logic<5> rs2_address;
  logic<32> rd_data;
  logic<32> rs1_data;
  logic<32> rs2_data;

 private:
  // 32 registers of 32-bit width
  logic<32> _register[32];

 public:
  // Read ports for rs1 and rs1
  void tock1() {
    rs1_data = _register[rs1_address];
    rs2_data = _register[rs2_address];
  }

  // Register x0 is always 0
  regfile() { _register[0] = b32(0b0); }

  void tock() { tick(); }

 private:
  // Write port for rd
  void tick() {
    if (write_enable)
      if (rd_address != b5(0b0)) _register[rd_address] = rd_data;
  }
};

#endif // REGFILE_H
