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
  logic<5>  reg_waddr;
  logic<32> reg_data;
  logic<32> pc;
};

struct registers_p0 {
  logic<2>  hart;
  logic<32> pc;
};

struct signals_p01 {
  logic<2>  hart;
  logic<32> pc;
  logic<32> insn;
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
  logic<7>  f7;
  logic<5>  rd;

  logic<32> dbus_addr;
  logic<32> dbus_data;
  logic<4>  dbus_mask;

  logic<32> alu;
  logic<32> imm;
  logic<32> ra;
  logic<32> rb;
};

struct registers_p2 {
  logic<2>  hart;
  logic<32> pc;
  logic<5>  op;
  logic<3>  f3;
  logic<7>  f7;
  logic<32> align;
  logic<32> alu;
  logic<5>  rd;
  logic<32> imm;
  logic<32> ra;
  logic<32> rb;
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
    reg_p2.alu    = 0;
    reg_p2.rd     = 0;

    tock01(reg_p0, 0,    sig_p01);
    tock12(reg_p1, 0, 0, sig_p12);
    tock20(reg_p2, 0,    sig_p20);
  }

  //----------------------------------------

  static void tock01(const registers_p0& reg_p0, logic<32> insn, signals_p01& sig_p01) {
    sig_p01.hart  = reg_p0.hart;
    sig_p01.pc    = reg_p0.pc;

    sig_p01.addr1 = b5(insn, 15);
    sig_p01.addr2 = b5(insn, 20);
  }

  //----------------------------------------

  static void tick01(const signals_p01& sig_p01, logic<32> insn, registers_p1& reg_p1) {
    reg_p1.hart = sig_p01.hart;
    reg_p1.pc   = sig_p01.pc;

    logic<32> imm_b = cat(dup<20>(insn[31]), insn[7], b6(insn, 25), b4(insn, 8), b1(0));
    logic<32> imm_i = cat(dup<21>(insn[31]), b6(insn, 25), b5(insn, 20));
    logic<32> imm_j = cat(dup<12>(insn[31]), b8(insn, 12), insn[20], b6(insn, 25), b4(insn, 21), b1(0));
    logic<32> imm_s = cat(dup<21>(insn[31]), b6(insn, 25), b5(insn, 7));
    logic<32> imm_u = cat(insn[31], b11(insn, 20), b8(insn, 12), b12(0));

    logic<32> imm;
    switch(b5(insn, 2)) {
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

    reg_p1.inst = insn;
    reg_p1.imm  = imm;
    reg_p1.op   = b5(insn, 2);
    reg_p1.f3   = b3(insn, 12);
    reg_p1.f7   = b7(insn, 25);
    reg_p1.rd   = b5(insn, 7);
  }

  //----------------------------------------

  static void tock12(const registers_p1& reg_p1, logic<32> ra, logic<32> rb, signals_p12& sig_p12) {
    sig_p12.hart   = reg_p1.hart;
    sig_p12.pc     = reg_p1.pc;

    // Data bus driver
    {
      sig_p12.dbus_addr = ra + reg_p1.imm;
      logic<2> align = b2(sig_p12.dbus_addr);
      sig_p12.dbus_data = rb << (8 * align);

      if (reg_p1.op == OP_STORE) {
        if      (reg_p1.f3 == 0) sig_p12.dbus_mask = 0b0001 << align;
        else if (reg_p1.f3 == 1) sig_p12.dbus_mask = 0b0011 << align;
        else if (reg_p1.f3 == 2) sig_p12.dbus_mask = 0b1111;
        else                     sig_p12.dbus_mask = 0b0000;
      }
      else {
        sig_p12.dbus_mask = 0b0000;
      }
    }

    sig_p12.op     = reg_p1.op;
    sig_p12.f3     = reg_p1.f3;
    sig_p12.f7     = reg_p1.f7;
    sig_p12.rd     = reg_p1.rd;
    sig_p12.imm    = reg_p1.imm;
  }

  //----------------------------------------

  static void tick12(const signals_p12& sig_p12, logic<32> ra, logic<32> rb, registers_p2& reg_p2) {
    reg_p2.hart   = sig_p12.hart;
    reg_p2.pc     = sig_p12.pc;

    reg_p2.op     = sig_p12.op;
    reg_p2.f3     = sig_p12.f3;
    reg_p2.f7     = sig_p12.f7;

    reg_p2.align  = b2(sig_p12.dbus_addr);
    reg_p2.rd     = sig_p12.rd;
    reg_p2.imm    = sig_p12.imm;
    reg_p2.ra     = ra;
    reg_p2.rb     = rb;
  }

  //----------------------------------------

  static void tock20(const registers_p2& reg_p2, const logic<32> dbus_data, signals_p20& sig_p20) {
    sig_p20.hart     = reg_p2.hart;

    // Branch selection
    {
      logic<1> eq  = reg_p2.ra == reg_p2.rb;
      logic<1> slt = signed(reg_p2.ra) < signed(reg_p2.rb);
      logic<1> ult = reg_p2.ra < reg_p2.rb;

      logic<1> pc_sel;
      switch (reg_p2.f3) {
        case 0: pc_sel =   eq; break;
        case 1: pc_sel =  !eq; break;
        case 2: pc_sel =   eq; break;
        case 3: pc_sel =  !eq; break;
        case 4: pc_sel =  slt; break;
        case 5: pc_sel = !slt; break;
        case 6: pc_sel =  ult; break;
        case 7: pc_sel = !ult; break;
      }

      switch(reg_p2.op) {
        case OP_ALU:     sig_p20.pc = reg_p2.pc + 4; break;
        case OP_ALUI:    sig_p20.pc = reg_p2.pc + 4; break;
        case OP_LOAD:    sig_p20.pc = reg_p2.pc + 4; break;
        case OP_STORE:   sig_p20.pc = reg_p2.pc + 4; break;
        case OP_BRANCH:  sig_p20.pc = pc_sel ? b32(reg_p2.pc + reg_p2.imm) : b32(reg_p2.pc + 4); break;
        case OP_JAL:     sig_p20.pc = reg_p2.pc + reg_p2.imm; break;
        case OP_JALR:    sig_p20.pc = reg_p2.ra + reg_p2.imm; break;
        case OP_LUI:     sig_p20.pc = reg_p2.pc + 4; break;
        case OP_AUIPC:   sig_p20.pc = reg_p2.pc + 4; break;
      }
    }

    // ALU
    logic<32> alu;
    {
      logic<32> ra = reg_p2.ra;
      logic<32> rb = reg_p2.rb;
      if (reg_p2.op == OP_ALU && reg_p2.f3 == 0 && b1(reg_p2.f7, 5)) rb = uint32_t(-rb);
      if (reg_p2.op == OP_ALUI) rb = reg_p2.imm;

      switch (reg_p2.f3) {
        case 0: alu = ra + rb; break;
        case 1: alu = sll(ra, b5(rb)); break;
        case 2: alu = signed(ra) < signed(rb); break;
        case 3: alu = ra < rb; break;
        case 4: alu = ra ^ rb; break;
        case 5: alu = b1(reg_p2.f7, 5) ? sra(ra, b5(rb)) : srl(ra, b5(rb)); break;
        case 6: alu = ra | rb; break;
        case 7: alu = ra & rb; break;
      }
    }

    // Writeback
    {
      logic<32> unpacked;
      switch (reg_p2.f3) {
        case 0: unpacked = sign_extend<32>(b8(dbus_data, 8 * reg_p2.align)); break;
        case 1: unpacked = sign_extend<32>(b16(dbus_data, 8 * reg_p2.align)); break;
        case 2: unpacked = dbus_data; break;
        case 3: unpacked = dbus_data; break;
        case 4: unpacked = b8(dbus_data, 8 * reg_p2.align); break;
        case 5: unpacked = b16(dbus_data, 8 * reg_p2.align); break;
        case 6: unpacked = dbus_data; break;
        case 7: unpacked = dbus_data; break;
      }

      switch(reg_p2.op) {
        case OP_ALU:     sig_p20.reg_waddr = reg_p2.rd; sig_p20.reg_data = alu;                     break;
        case OP_ALUI:    sig_p20.reg_waddr = reg_p2.rd; sig_p20.reg_data = alu;                     break;
        case OP_LOAD:    sig_p20.reg_waddr = reg_p2.rd; sig_p20.reg_data = unpacked;                break;
        case OP_STORE:   sig_p20.reg_waddr = 0;         sig_p20.reg_data = b32(DONTCARE);           break;
        case OP_BRANCH:  sig_p20.reg_waddr = 0;         sig_p20.reg_data = b32(DONTCARE);           break;
        case OP_JAL:     sig_p20.reg_waddr = reg_p2.rd; sig_p20.reg_data = reg_p2.pc + 4;           break;
        case OP_JALR:    sig_p20.reg_waddr = reg_p2.rd; sig_p20.reg_data = reg_p2.pc + 4;           break;
        case OP_LUI:     sig_p20.reg_waddr = reg_p2.rd; sig_p20.reg_data = reg_p2.imm;              break;
        case OP_AUIPC:   sig_p20.reg_waddr = reg_p2.rd; sig_p20.reg_data = reg_p2.pc + reg_p2.imm;  break;
      }
    }
  }

  //----------------------------------------

  static void tick20(const signals_p20& sig_p20, registers_p0& reg_p0) {
    reg_p0.hart = sig_p20.hart;
    reg_p0.pc   = sig_p20.pc;
  }

  //----------------------------------------

  void tick(logic<1> reset_in) {
    if (reset_in) {
      reset();
      return;
    }

    logic<32> dbus_data = data_mem[cat(sig_p12.hart, b10(sig_p12.dbus_addr, 2))];
    logic<32> pbus_data = code_mem[b10(sig_p20.pc, 2)];

    logic<32> ra = regfile[cat(sig_p01.hart, sig_p01.addr1)];
    logic<32> rb = regfile[cat(sig_p01.hart, sig_p01.addr2)];

    tock01(reg_p0,  pbus_data, sig_p01);
    tock12(reg_p1,  ra, rb,    sig_p12);
    tock20(reg_p2,  dbus_data, sig_p20);

    tick01(sig_p01, pbus_data, reg_p1);
    tick12(sig_p12, ra, rb,    reg_p2);
    tick20(sig_p20,            reg_p0);

    if (sig_p12.dbus_mask) {
      logic<32> data = data_mem[cat(sig_p12.hart, b10(sig_p12.dbus_addr, 2))];
      if (sig_p12.dbus_mask[0]) data = (data & 0xFFFFFF00) | (sig_p12.dbus_data & 0x000000FF);
      if (sig_p12.dbus_mask[1]) data = (data & 0xFFFF00FF) | (sig_p12.dbus_data & 0x0000FF00);
      if (sig_p12.dbus_mask[2]) data = (data & 0xFF00FFFF) | (sig_p12.dbus_data & 0x00FF0000);
      if (sig_p12.dbus_mask[3]) data = (data & 0x00FFFFFF) | (sig_p12.dbus_data & 0xFF000000);
      data_mem[cat(sig_p12.hart, b10(sig_p12.dbus_addr, 2))] = data;
    }

    if (sig_p20.reg_waddr) {
      regfile[cat(sig_p20.hart, sig_p20.reg_waddr)] = sig_p20.reg_data;
    }
  }
};
