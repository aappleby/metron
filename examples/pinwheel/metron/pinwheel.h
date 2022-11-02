#pragma once
#include "metron_tools.h"
#include "constants.h"

static const int OP_ALU    = 0b01100;
static const int OP_ALUI   = 0b00100;
static const int OP_LOAD   = 0b00000;
static const int OP_STORE  = 0b01000;
static const int OP_BRANCH = 0b11000;
static const int OP_JAL    = 0b11011;
static const int OP_JALR   = 0b11001;
static const int OP_LUI    = 0b01101;
static const int OP_AUIPC  = 0b00101;

inline const char* op_id_to_name(int op_id) {
  switch(op_id) {
    case OP_ALU:    return "OP_ALU   ";
    case OP_ALUI:   return "OP_ALUI  ";
    case OP_LOAD:   return "OP_LOAD  ";
    case OP_STORE:  return "OP_STORE ";
    case OP_BRANCH: return "OP_BRANCH";
    case OP_JAL:    return "OP_JAL   ";
    case OP_JALR:   return "OP_JALR  ";
    case OP_LUI:    return "OP_LUI   ";
    case OP_AUIPC:  return "OP_AUIPC ";
    default:        return "???      ";
  }
}

//------------------------------------------------------------------------------

struct signals_p20 {
  logic<2>  hart;
  logic<5>  reg_addr;
  logic<32> reg_data;
  logic<1>  reg_wren;
  logic<32> code_addr;
};

struct registers_p0 {
  logic<2>  hart;
  logic<32> pc;
};

struct signals_p01 {
  logic<2>  hart;
  logic<32> pc;
  logic<32> code;
  logic<5>  addr1;
  logic<5>  addr2;
};

struct registers_p1 {
  logic<2>  hart;
  logic<32> pc;
  logic<32> inst;
  logic<32> imm;
  logic<5>  op;
  logic<3>  f3;
  logic<7>  f7;
  logic<5>  rd;
};

struct signals_p12 {
  logic<2>  hart;
  logic<32> pc;
  logic<5>  op;
  logic<3>  f3;
  logic<5>  rd;
  logic<32> adder;
  logic<32> data;
  logic<4>  mask;
  logic<32> alu;
  logic<1>  pc_sel;
};

struct registers_p2 {
  logic<2>  hart;
  logic<32> pc;
  logic<5>  op;
  logic<3>  f3;
  logic<1>  pc_sel;
  logic<32> adder;
  logic<32> alu;
  logic<5>  rd;
};

//------------------------------------------------------------------------------

class Pinwheel {
 public:
  void tock(logic<1> reset) {
    tick(reset);
  }

  logic<32> code_mem[1024];     // Cores share a 4K ROM
  logic<32> data_mem[1024 * 4]; // Cores have their own 4K RAM
  logic<32> regfile[32*4];      // Cores have their own register file

  registers_p0 reg_p0;
  signals_p01  sig_p01;
  registers_p1 reg_p1;
  signals_p12  sig_p12;
  registers_p2 reg_p2;
  signals_p20  sig_p20;

  void dump() {
    if (reg_p0.hart == 0) {
      printf("h%dp0: 0x%08x\n", (int)reg_p0.hart, (int)reg_p0.pc);
    }
    if (reg_p1.hart == 0) {
      printf("h%dp1: 0x%08x %s inst 0x%08x imm 0x%08x\n", (int)reg_p1.hart, (int)reg_p1.pc, op_id_to_name(reg_p1.op), (uint32_t)reg_p1.inst, (uint32_t)reg_p1.imm);
    }
    if (reg_p2.hart == 0) {
      printf("h%dp2: 0x%08x %s alu 0x%08x\n", (int)reg_p2.hart, (int)reg_p2.pc, op_id_to_name(reg_p2.op), (uint32_t)reg_p2.alu);
    }
  }

  //----------------------------------------

  void reset() {
    std::string s;
    value_plusargs("text_file=%s", s);
    readmemh(s, code_mem);

    value_plusargs("data_file=%s", s);
    readmemh(s, data_mem + 1024 * 0);
    readmemh(s, data_mem + 1024 * 1);
    readmemh(s, data_mem + 1024 * 2);
    readmemh(s, data_mem + 1024 * 3);

    memset(regfile, 0, sizeof(regfile));

    reg_p0.hart   = 2;
    reg_p0.pc     = -4;

    reg_p1.hart   = 1;
    reg_p1.pc     = -4;
    reg_p1.inst   = 0;
    reg_p1.op     = 0;
    reg_p1.imm    = 0;
    reg_p1.f3     = 0;
    reg_p1.f7     = 0;
    reg_p1.rd     = 0;

    reg_p2.hart   = 0;
    reg_p2.pc     = -4;
    reg_p2.op     = 0;
    reg_p2.f3     = 0;
    reg_p2.pc_sel = 0;
    reg_p2.alu    = 0;
    reg_p2.rd     = 0;

    tock01(reg_p0, 0,    sig_p01);
    tock12(reg_p1, 0, 0, sig_p12);
    tock20(reg_p2, 0,    sig_p20);
  }

  //----------------------------------------

  static void tock01(const registers_p0& reg_p0, logic<32> code, signals_p01& sig_p01) {
    sig_p01.hart  = reg_p0.hart;
    sig_p01.pc    = reg_p0.pc;
    sig_p01.code  = code;
    sig_p01.addr1 = b5(code, 15);
    sig_p01.addr2 = b5(code, 20);

    if (b5(code, 2) == OP_LUI) sig_p01.addr1 = 0;
  }

  //----------------------------------------

  static void tick01(const signals_p01& sig_p01, registers_p1& reg_p1) {
    logic<32> code = sig_p01.code;
    logic<32> imm_b = cat(dup<20>(code[31]), code[7], b6(code, 25), b4(code, 8), b1(0));
    logic<32> imm_i = cat(dup<21>(code[31]), b6(code, 25), b5(code, 20));
    logic<32> imm_j = cat(dup<12>(code[31]), b8(code, 12), code[20], b6(code, 25), b4(code, 21), b1(0));
    logic<32> imm_s = cat(dup<21>(code[31]), b6(code, 25), b5(code, 7));
    logic<32> imm_u = cat(code[31], b11(code, 20), b8(code, 12), b12(0));

    logic<32> imm;
    switch(b5(code, 2)) {
      case OP_ALU:    imm = imm_i; break;
      case OP_ALUI:   imm = imm_i; break;
      case OP_LOAD:   imm = imm_i; break;
      case OP_STORE:  imm = imm_s; break;
      case OP_BRANCH: imm = imm_b; break;
      case OP_JAL:    imm = imm_j; break;
      case OP_JALR:   imm = imm_i; break;
      case OP_LUI:    imm = imm_u; break;
      case OP_AUIPC:  imm = imm_u; break;
    }

    reg_p1.hart = sig_p01.hart;
    reg_p1.pc   = sig_p01.pc;
    reg_p1.inst = code;
    reg_p1.imm  = imm;
    reg_p1.op   = b5(code, 2);
    reg_p1.f3   = b3(code, 12);
    reg_p1.f7   = b7(code, 25);
    reg_p1.rd   = b5(code, 7);
  }

  //----------------------------------------

  static void tock12(const registers_p1& reg_p1, logic<32> ra, logic<32> rb,
                      signals_p12& sig_p12) {
    // Branch selection
    logic<1> pc_sel;
    switch(reg_p1.op) {
      case OP_ALU:     pc_sel = 0; break;
      case OP_ALUI:    pc_sel = 0; break;
      case OP_LOAD:    pc_sel = 0; break;
      case OP_STORE:   pc_sel = 0; break;
      case OP_BRANCH:  {
        logic<1> eq  = ra == rb;
        logic<1> slt = signed(ra) < signed(rb);
        logic<1> ult = ra < rb;
        logic<1> take_branch;
        switch (reg_p1.f3) {
          case 0: pc_sel =   eq; break;
          case 1: pc_sel =  !eq; break;
          case 2: pc_sel =   eq; break;
          case 3: pc_sel =  !eq; break;
          case 4: pc_sel =  slt; break;
          case 5: pc_sel = !slt; break;
          case 6: pc_sel =  ult; break;
          case 7: pc_sel = !ult; break;
        }
        break;
      }
      case OP_JAL:     pc_sel = 1; break;
      case OP_JALR:    pc_sel = 1; break;
      case OP_LUI:     pc_sel = 0; break;
      case OP_AUIPC:   pc_sel = 0; break;
    }

    // ALU

    logic<32> adder;
    switch(reg_p1.op) {
      case OP_ALU:     adder = ra        + reg_p1.imm; break;
      case OP_ALUI:    adder = ra        + reg_p1.imm; break;
      case OP_LOAD:    adder = ra        + reg_p1.imm; break;
      case OP_STORE:   adder = ra        + reg_p1.imm; break;
      case OP_JAL:     adder = reg_p1.pc + reg_p1.imm; break;
      case OP_BRANCH:  adder = reg_p1.pc + reg_p1.imm; break;
      case OP_JALR:    adder = ra        + reg_p1.imm; break;
      case OP_LUI:     adder = ra        + reg_p1.imm; break;
      case OP_AUIPC:   adder = reg_p1.pc + reg_p1.imm; break;
    }

    logic<3>  alu_op;
    logic<32> alu_a = ra;
    logic<32> alu_b = rb;

    switch(reg_p1.op) {
      case OP_ALU:     alu_op = reg_p1.f3; alu_a = ra;        alu_b = (reg_p1.f3 == 0 && b1(reg_p1.f7, 5)) ? uint32_t(-alu_b) : uint32_t(alu_b); break;
      case OP_ALUI:    alu_op = reg_p1.f3; alu_a = ra;        alu_b = reg_p1.imm; break;
      case OP_LOAD:    alu_op = 0;         alu_a = reg_p1.pc; alu_b = 4; break;
      case OP_STORE:   alu_op = 0;         alu_a = reg_p1.pc; alu_b = 4; break;
      case OP_JAL:     alu_op = 0;         alu_a = reg_p1.pc; alu_b = 4; break;
      case OP_BRANCH:  alu_op = 0;         alu_a = reg_p1.pc; alu_b = 4; break;
      case OP_JALR:    alu_op = 0;         alu_a = reg_p1.pc; alu_b = 4; break;
      case OP_LUI:     alu_op = 0;         alu_a = reg_p1.pc; alu_b = 4; break;
      case OP_AUIPC:   alu_op = 0;         alu_a = reg_p1.pc; alu_b = 4; break;
    }

    logic<32> alu;
    switch (alu_op) {
      case 0: alu = alu_a + alu_b; break;
      case 1: alu = sll(alu_a, b5(alu_b)); break;
      case 2: alu = signed(alu_a) < signed(alu_b); break;
      case 3: alu = alu_a < alu_b; break;
      case 4: alu = alu_a ^ alu_b; break;
      case 5: alu = b1(reg_p1.f7, 5) ? sra(alu_a, b5(alu_b)) : srl(alu_a, b5(alu_b)); break;
      case 6: alu = alu_a | alu_b; break;
      case 7: alu = alu_a & alu_b; break;
    }

    // Data mem read/write
    logic<32> data = rb << (8 * b2(adder));
    logic<4>  mask = 0b0000;

    if (reg_p1.op == OP_STORE) {
      if (reg_p1.f3 == 0) mask = 0b0001 << b2(adder);
      if (reg_p1.f3 == 1) mask = 0b0011 << b2(adder);
      if (reg_p1.f3 == 2) mask = 0b1111;
    }

    sig_p12.hart   = reg_p1.hart;
    sig_p12.pc     = reg_p1.pc;
    sig_p12.op     = reg_p1.op;
    sig_p12.f3     = reg_p1.f3;
    sig_p12.rd     = reg_p1.rd;
    sig_p12.adder  = adder;
    sig_p12.data   = data;
    sig_p12.mask   = mask;
    sig_p12.alu    = alu;
    sig_p12.pc_sel = pc_sel;
  }

  //----------------------------------------

  static void tick12(const signals_p12& sig_p12, registers_p2& reg_p2) {
    reg_p2.hart   = sig_p12.hart;
    reg_p2.pc     = sig_p12.pc;
    reg_p2.op     = sig_p12.op;
    reg_p2.f3     = sig_p12.f3;
    reg_p2.pc_sel = sig_p12.pc_sel;
    reg_p2.adder  = sig_p12.adder;
    reg_p2.alu    = sig_p12.alu;
    reg_p2.rd     = sig_p12.rd;
  }

  //----------------------------------------

  static void tock20(const registers_p2& reg_p2, const logic<32> data, signals_p20& sig_p20) {
    logic<32> unpacked;
    switch (reg_p2.f3) {
      case 0: unpacked = sign_extend<32>(b8(data, 8 * b2(reg_p2.adder))); break;
      case 1: unpacked = sign_extend<32>(b16(data, 8 * b2(reg_p2.adder))); break;
      case 2: unpacked = data; break;
      case 3: unpacked = data; break;
      case 4: unpacked = b8(data, 8 * b2(reg_p2.adder)); break;
      case 5: unpacked = b16(data, 8 * b2(reg_p2.adder)); break;
      case 6: unpacked = data; break;
      case 7: unpacked = data; break;
    }

    logic<1>  reg_wren;
    logic<32> reg_data;
    switch(reg_p2.op) {
      case OP_ALU:     reg_wren = true;  reg_data = reg_p2.alu;    break;
      case OP_ALUI:    reg_wren = true;  reg_data = reg_p2.alu;    break;
      case OP_LOAD:    reg_wren = true;  reg_data = unpacked;      break;
      case OP_STORE:   reg_wren = false; reg_data = b32(DONTCARE); break;
      case OP_BRANCH:  reg_wren = false; reg_data = b32(DONTCARE); break;
      case OP_JAL:     reg_wren = true;  reg_data = reg_p2.alu;    break;
      case OP_JALR:    reg_wren = true;  reg_data = reg_p2.alu;    break;
      case OP_LUI:     reg_wren = true;  reg_data = reg_p2.adder;  break;
      case OP_AUIPC:   reg_wren = true;  reg_data = reg_p2.adder;  break;
    }

    logic<32> new_pc = reg_p2.pc_sel ? b32(reg_p2.adder) : b32(reg_p2.pc + 4);

    sig_p20.hart     = reg_p2.hart;
    sig_p20.reg_addr = reg_p2.rd;
    sig_p20.reg_data = reg_data;
    sig_p20.reg_wren = reg_wren;
    sig_p20.code_addr = new_pc;
  }

  //----------------------------------------

  static void tick20(const signals_p20& sig_p20, registers_p0& reg_p0) {
    reg_p0.hart = sig_p20.hart;
    reg_p0.pc   = sig_p20.code_addr;
  }

  //----------------------------------------

  void tick(logic<1> reset_in) {
    if (reset_in) {
      reset();
      return;
    }

    logic<32> data = data_mem[cat(sig_p12.hart, b10(sig_p12.adder, 2))];
    logic<32> code = code_mem[b10(sig_p20.code_addr, 2)];

    logic<32> ra = regfile[cat(sig_p01.hart, sig_p01.addr1)];
    logic<32> rb = regfile[cat(sig_p01.hart, sig_p01.addr2)];

    tock01(reg_p0, code,   sig_p01);
    tock12(reg_p1, ra, rb, sig_p12);
    tock20(reg_p2, data,   sig_p20);

    tick20(sig_p20, reg_p0);
    tick01(sig_p01, reg_p1);
    tick12(sig_p12, reg_p2);

    if (sig_p12.mask) {
      logic<32> data = data_mem[cat(sig_p12.hart, b10(sig_p12.adder, 2))];
      if (sig_p12.mask[0]) data = (data & 0xFFFFFF00) | (sig_p12.data & 0x000000FF);
      if (sig_p12.mask[1]) data = (data & 0xFFFF00FF) | (sig_p12.data & 0x0000FF00);
      if (sig_p12.mask[2]) data = (data & 0xFF00FFFF) | (sig_p12.data & 0x00FF0000);
      if (sig_p12.mask[3]) data = (data & 0x00FFFFFF) | (sig_p12.data & 0xFF000000);
      data_mem[cat(sig_p12.hart, b10(sig_p12.adder, 2))] = data;
    }

    if (sig_p20.reg_addr && sig_p20.reg_wren) {
      regfile[cat(sig_p20.hart, sig_p20.reg_addr)] = sig_p20.reg_data;
    }
  }
};
