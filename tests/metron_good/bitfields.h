#include "metron/metron_tools.h"

// C structs with bit-size annotations become arrays of sized logic<>s.

struct rv32_rtype {
  uint32_t op  : 7;
  uint32_t rd  : 5;
  uint32_t f3  : 3;
  uint32_t rs1 : 5;
  uint32_t rs2 : 5;
  uint32_t f7  : 7;
};

struct rv32_itype {
  uint32_t op : 7;
  uint32_t rd : 5;
  uint32_t f3 : 3;
  uint32_t rs1 : 5;
  uint32_t imm_11_0 : 12;
};

struct rv32_stype {
  uint32_t op : 7;
  uint32_t imm_4_0 : 5;
  uint32_t f3 : 3;
  uint32_t rs1 : 5;
  uint32_t rs2 : 5;
  uint32_t imm_11_5 : 7;
};

struct rv32_btype {
  uint32_t op : 7;
  uint32_t imm_11 : 1;
  uint32_t imm_4_1 : 4;
  uint32_t f3 : 3;
  uint32_t rs1 : 5;
  uint32_t rs2 : 5;
  uint32_t imm_10_5 : 6;
  uint32_t imm_12 : 1;
};

struct rv32_utype {
  uint32_t op : 7;
  uint32_t rd : 5;
  uint32_t imm_31_12 : 20;
};

struct rv32_jtype {
  uint32_t op : 7;
  uint32_t rd : 5;
  uint32_t imm_19_12 : 8;
  uint32_t imm_11 : 1;
  uint32_t imm_10_1 : 10;
  uint32_t imm_20: 1;
};

union rv32_insn {
  logic<32> raw;
  rv32_rtype r;
  rv32_itype i;
  rv32_stype s;
  rv32_btype b;
  rv32_utype u;
  rv32_jtype j;
};

class Module {
  public:
};
