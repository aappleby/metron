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
  logic<32> pc;

  logic<5>  rbus_waddr;
  logic<32> rbus_wdata;
};

struct registers_p0 {
  logic<2>  hart;
  logic<32> pc;
};

struct signals_p01 {
  logic<2>  hart;
  logic<32> pc;
  logic<32> insn;

  logic<5>  rbus_raddr1;
  logic<5>  rbus_raddr2;
};

struct registers_p1 {
  logic<2>  hart;
  logic<32> pc;
  logic<32> insn;
};

struct signals_p12 {
  logic<2>  hart;
  logic<32> pc;
  logic<32> insn;

  logic<32> dbus_addr;
  logic<2>  dbus_align;
  logic<32> dbus_wdata;
  logic<4>  dbus_wmask;
  logic<32> alu;
  logic<32> ra;
  logic<32> rb;
};

struct registers_p2 {
  logic<2>  hart;
  logic<32> pc;
  logic<32> insn;

  logic<2>  dbus_align;
  logic<32> ra;
  logic<32> rb;
};

//------------------------------------------------------------------------------

class pinwheel {
 public:
  void tock(logic<1> reset) {
    tick(reset);
  }

  uint32_t code_mem[1024];    // Cores share a 4K ROM
  uint32_t data_mem[4][1024]; // Cores have their own 4K RAM
  uint32_t regfile[4][32];    // Cores have their own register file

  registers_p0 reg_p0;
  signals_p01  sig_p01;
  registers_p1 reg_p1;
  signals_p12  sig_p12;
  registers_p2 reg_p2;
  signals_p20  sig_p20;

  void dump() {
    if (reg_p0.hart == 0) {
      //printf("h%dp0: 0x%08x\n", (int)reg_p0.hart, (int)reg_p0.pc);
    }
    if (reg_p1.hart == 0) {
      //printf("h%dp1: 0x%08x %s inst 0x%08x imm 0x%08x\n", (int)reg_p1.hart, (int)reg_p1.pc, op_id_to_name(reg_p1.op), (uint32_t)reg_p1.insn, (uint32_t)reg_p1.imm);
    }
    if (reg_p2.hart == 0) {
      //printf("h%dp2: 0x%08x %s alu 0x%08x\n", (int)reg_p2.hart, (int)reg_p2.pc, op_id_to_name(reg_p2.op), (uint32_t)reg_p2.alu);
    }
  }

  //----------------------------------------

  void reset() {
    std::string s;
    value_plusargs("text_file=%s", s);
    readmemh(s, code_mem);

    value_plusargs("data_file=%s", s);
    readmemh(s, data_mem[0]);
    readmemh(s, data_mem[1]);
    readmemh(s, data_mem[2]);
    readmemh(s, data_mem[3]);

    memset(regfile, 0, sizeof(regfile));

    reg_p0.hart   = 2;
    reg_p0.pc     = -4;

    reg_p1.hart   = 1;
    reg_p1.pc     = -4;
    reg_p1.insn   = 0;

    reg_p2.hart   = 0;
    reg_p2.pc     = -4;
    reg_p2.insn   = 0;

    tock01(reg_p0, 0,    sig_p01);
    tock12(reg_p1, 0, 0, sig_p12);
    tock20(reg_p2, 0,    sig_p20);
  }

  //--------------------------------------------------------------------------------

  static void tock01(const registers_p0& reg_p0, logic<32> insn, signals_p01& sig_p01) {
    sig_p01.hart  = reg_p0.hart;
    sig_p01.pc    = reg_p0.pc;
    sig_p01.insn  = insn;

    sig_p01.rbus_raddr1 = b5(insn, 15);
    sig_p01.rbus_raddr2 = b5(insn, 20);
  }

  //----------------------------------------

  static void tick01(const signals_p01& sig_p01, registers_p1& reg_p1) {
    reg_p1.hart = sig_p01.hart;
    reg_p1.pc   = sig_p01.pc;
    reg_p1.insn = sig_p01.insn;
  }

  //--------------------------------------------------------------------------------

  static void tock12(const registers_p1& reg_p1, logic<32> ra, logic<32> rb, signals_p12& sig_p12) {
    sig_p12.hart   = reg_p1.hart;
    sig_p12.pc     = reg_p1.pc;
    sig_p12.insn   = reg_p1.insn;

    logic<32> insn = reg_p1.insn;
    logic<5> op = b5(insn, 2);
    logic<3> f3 = b3(insn, 12);

    // Data bus driver
    logic<32> imm_i = cat(dup<21>(insn[31]), b6(insn, 25), b5(insn, 20));
    logic<32> imm_s = cat(dup<21>(insn[31]), b6(insn, 25), b5(insn, 7));
    logic<32> addr = ra + ((op == OP_STORE) ? imm_s : imm_i);
    logic<2>  align = b2(addr);

    sig_p12.dbus_addr = addr;
    sig_p12.dbus_wdata = rb << (8 * align);
    sig_p12.dbus_align = align;

    if (op == OP_STORE) {
      if      (f3 == 0) sig_p12.dbus_wmask = 0b0001 << align;
      else if (f3 == 1) sig_p12.dbus_wmask = 0b0011 << align;
      else if (f3 == 2) sig_p12.dbus_wmask = 0b1111;
      else              sig_p12.dbus_wmask = 0b0000;
    }
    else {
      sig_p12.dbus_wmask = 0b0000;
    }
  }

  //----------------------------------------

  static void tick12(const signals_p12& sig_p12, logic<32> ra, logic<32> rb, registers_p2& reg_p2) {
    reg_p2.hart   = sig_p12.hart;
    reg_p2.pc     = sig_p12.pc;
    reg_p2.insn   = sig_p12.insn;

    reg_p2.dbus_align  = sig_p12.dbus_align;
    reg_p2.ra     = ra;
    reg_p2.rb     = rb;
  }

  //--------------------------------------------------------------------------------

  static void tock20(const registers_p2& reg_p2, const logic<32> dbus_data, signals_p20& sig_p20) {
    sig_p20.hart = reg_p2.hart;

    logic<32> insn = reg_p2.insn;
    logic<5> op = b5(insn, 2);
    logic<3> f3 = b3(insn, 12);
    logic<5> rd = b5(insn, 7);

    logic<32> imm_b = cat(dup<20>(insn[31]), insn[7], b6(insn, 25), b4(insn, 8), b1(0));
    logic<32> imm_i = cat(dup<21>(insn[31]), b6(insn, 25), b5(insn, 20));
    logic<32> imm_j = cat(dup<12>(insn[31]), b8(insn, 12), insn[20], b6(insn, 25), b4(insn, 21), b1(0));
    logic<32> imm_u = cat(insn[31], b11(insn, 20), b8(insn, 12), b12(0));

    // ALU
    logic<1>  alu_alt = b1(reg_p2.insn, 30);
    logic<32> alu_a  = reg_p2.ra;
    logic<32> alu_b  = reg_p2.rb;
    logic<3>  alu_op = f3;

    switch(op) {
      case OP_ALU:    alu_op = f3;        alu_a = reg_p2.ra; alu_b = (f3 == 0 && alu_alt) ? b32(-reg_p2.rb) : reg_p2.rb; break;
      case OP_ALUI:   alu_op = f3;        alu_a = reg_p2.ra; alu_b = imm_i; break;
      case OP_LOAD:   alu_op = f3;        alu_a = reg_p2.ra; alu_b = reg_p2.rb;  break;
      case OP_STORE:  alu_op = f3;        alu_a = reg_p2.ra; alu_b = reg_p2.rb;  break;
      case OP_BRANCH: alu_op = f3;        alu_a = reg_p2.ra; alu_b = reg_p2.rb;  break;
      case OP_JAL:    alu_op = 0;         alu_a = reg_p2.pc; alu_b = b32(4);     break;
      case OP_JALR:   alu_op = 0;         alu_a = reg_p2.pc; alu_b = b32(4);     break;
      case OP_LUI:    alu_op = 0;         alu_a = 0;         alu_b = imm_u; break;
      case OP_AUIPC:  alu_op = 0;         alu_a = reg_p2.pc; alu_b = imm_u; break;
    }

    // Shifters

    logic<1> bit = (alu_op == 5) && alu_alt ? alu_a[31] : b1(0);

    logic<32> sl = alu_a;
    if (alu_b[4]) sl = cat(b16(sl, 0), dup<16>(bit));
    if (alu_b[3]) sl = cat(b24(sl, 0), dup< 8>(bit));
    if (alu_b[2]) sl = cat(b28(sl, 0), dup< 4>(bit));
    if (alu_b[1]) sl = cat(b30(sl, 0), dup< 2>(bit));
    if (alu_b[0]) sl = cat(b31(sl, 0), dup< 1>(bit));

    logic<32> sr = alu_a;
    if (alu_b[4]) sr = cat(dup<16>(bit), b16(sr, 16));
    if (alu_b[3]) sr = cat(dup< 8>(bit), b24(sr,  8));
    if (alu_b[2]) sr = cat(dup< 4>(bit), b28(sr,  4));
    if (alu_b[1]) sr = cat(dup< 2>(bit), b30(sr,  2));
    if (alu_b[0]) sr = cat(dup< 1>(bit), b31(sr,  1));

    logic<32> alu_out;
    switch (alu_op) {
      case 0: alu_out = alu_a + alu_b;                 break;
      case 1: alu_out = sl;                            break;
      case 2: alu_out = signed(alu_a) < signed(alu_b); break;
      case 3: alu_out = alu_a < alu_b;                 break;
      case 4: alu_out = alu_a ^ alu_b;                 break;
      case 5: alu_out = sr;                            break;
      case 6: alu_out = alu_a | alu_b;                 break;
      case 7: alu_out = alu_a & alu_b;                 break;
    }

    // Writeback
    logic<32> unpacked; // Unpack byte/word from memory dword
    switch (f3) {
      case 0: unpacked = sign_extend<32>(b8(dbus_data, 8 * reg_p2.dbus_align)); break;
      case 1: unpacked = sign_extend<32>(b16(dbus_data, 8 * reg_p2.dbus_align)); break;
      case 2: unpacked = dbus_data; break;
      case 3: unpacked = dbus_data; break;
      case 4: unpacked = b8(dbus_data, 8 * reg_p2.dbus_align); break;
      case 5: unpacked = b16(dbus_data, 8 * reg_p2.dbus_align); break;
      case 6: unpacked = dbus_data; break;
      case 7: unpacked = dbus_data; break;
    }
    sig_p20.rbus_wdata = op == OP_LOAD ? unpacked : alu_out;
    sig_p20.rbus_waddr = rd;
    if (op == OP_STORE)  sig_p20.rbus_waddr = 0;
    if (op == OP_BRANCH) sig_p20.rbus_waddr = 0;

    // Next PC
    logic<1> eq  = reg_p2.ra == reg_p2.rb;
    logic<1> slt = signed(reg_p2.ra) < signed(reg_p2.rb);
    logic<1> ult = reg_p2.ra < reg_p2.rb;
    logic<1> jump_rel = 0;
    if (op == OP_BRANCH) {
      switch (f3) {
        case 0: jump_rel =   eq; break;
        case 1: jump_rel =  !eq; break;
        case 2: jump_rel =   eq; break;
        case 3: jump_rel =  !eq; break;
        case 4: jump_rel =  slt; break;
        case 5: jump_rel = !slt; break;
        case 6: jump_rel =  ult; break;
        case 7: jump_rel = !ult; break;
      }
    }
    else if (op == OP_JAL)  jump_rel = 1;
    else if (op == OP_JALR) jump_rel = 1;

    logic<32> pc_lhs = reg_p2.pc;
    logic<32> pc_rhs = b32(4);
    if (op == OP_BRANCH) pc_rhs = jump_rel ? imm_b : b32(4);
    if (op == OP_JAL)    pc_rhs = imm_j;
    if (op == OP_JALR)   pc_lhs = reg_p2.ra;
    if (op == OP_JALR)   pc_rhs = imm_i;
    sig_p20.pc = pc_lhs + pc_rhs;
  }

  //----------------------------------------

  static void tick20(const signals_p20& sig_p20, registers_p0& reg_p0) {
    reg_p0.hart = sig_p20.hart;
    reg_p0.pc   = sig_p20.pc;
  }

  //--------------------------------------------------------------------------------

  void tick(logic<1> reset_in) {
    if (reset_in) {
      reset();
      return;
    }

    // Progmem
    logic<32> pbus_data = code_mem[b10(sig_p20.pc, 2)];

    // Datamem
    logic<32> dbus_data = data_mem[sig_p12.hart][b10(sig_p12.dbus_addr, 2)];
    if (sig_p12.dbus_wmask) {
      if (sig_p12.dbus_wmask[0]) dbus_data = (dbus_data & 0xFFFFFF00) | (sig_p12.dbus_wdata & 0x000000FF);
      if (sig_p12.dbus_wmask[1]) dbus_data = (dbus_data & 0xFFFF00FF) | (sig_p12.dbus_wdata & 0x0000FF00);
      if (sig_p12.dbus_wmask[2]) dbus_data = (dbus_data & 0xFF00FFFF) | (sig_p12.dbus_wdata & 0x00FF0000);
      if (sig_p12.dbus_wmask[3]) dbus_data = (dbus_data & 0x00FFFFFF) | (sig_p12.dbus_wdata & 0xFF000000);
      data_mem[sig_p12.hart][b10(sig_p12.dbus_addr, 2)] = dbus_data;
    }

    // Regfile
    logic<32> ra = regfile[sig_p01.hart][sig_p01.rbus_raddr1];
    logic<32> rb = regfile[sig_p01.hart][sig_p01.rbus_raddr2];
    if (sig_p20.rbus_waddr) {
      regfile[sig_p20.hart][sig_p20.rbus_waddr] = sig_p20.rbus_wdata;
    }

    tock01(reg_p0,  pbus_data, sig_p01);
    tock12(reg_p1,  ra, rb,    sig_p12);
    tock20(reg_p2,  dbus_data, sig_p20);

    tick01(sig_p01,            reg_p1);
    tick12(sig_p12, ra, rb,    reg_p2);
    tick20(sig_p20,            reg_p0);
  }
};
