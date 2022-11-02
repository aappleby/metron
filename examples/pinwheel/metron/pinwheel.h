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

struct registers_p0 {
  logic<2>  hart;
  logic<32> pc;
};

struct signals_p20 {
  logic<2>  hart;
  logic<5>  reg_addr;
  logic<32> reg_data;
  logic<1>  reg_wren;
  logic<32> mem_addr;
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

struct signals_p01 {
  logic<2>  hart;
  logic<5> addr1;
  logic<5> addr2;
};

struct registers_p2 {
  logic<2>  hart;
  logic<32> pc;
  logic<5>  op;
  logic<3>  f3;
  logic<1>  pc_sel;
  logic<32> alu;
  logic<5>  rd;
};

struct signals_p12 {
  logic<2>  hart;
  logic<32> addr;
  logic<32> data;
  logic<4>  byte_wren;
};



class Pinwheel {
 public:
  void tock(logic<1> reset) {
    tick(reset);
  }

  logic<32> prog_mem[1024];     // Cores share a 4K ROM
  logic<32> data_mem[1024 * 4]; // Cores have their own 4K RAM
  logic<32> regfile[32*4];      // Cores have their own register file

  registers_p0 reg_p0;
  registers_p1 reg_p1;
  registers_p2 reg_p2;

  signals_p20 sig_p20;
  signals_p01 sig_p01;
  signals_p12 sig_p12;

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
    readmemh(s, prog_mem);

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

    sig_p20 = {0};
    sig_p01 = {0};
    sig_p12 = {0};
  }

  //----------------------------------------

  static void delta01(const registers_p0& reg_p0, logic<32> p0_prog_read, signals_p01& sig_p01, registers_p1& reg_p1) {
    logic<32> imm_b = cat(dup<20>(p0_prog_read[31]), p0_prog_read[7], b6(p0_prog_read, 25), b4(p0_prog_read, 8), b1(0));
    logic<32> imm_i = cat(dup<21>(p0_prog_read[31]), b6(p0_prog_read, 25), b5(p0_prog_read, 20));
    logic<32> imm_j = cat(dup<12>(p0_prog_read[31]), b8(p0_prog_read, 12), p0_prog_read[20], b6(p0_prog_read, 25), b4(p0_prog_read, 21), b1(0));
    logic<32> imm_s = cat(dup<21>(p0_prog_read[31]), b6(p0_prog_read, 25), b5(p0_prog_read, 7));
    logic<32> imm_u = cat(p0_prog_read[31], b11(p0_prog_read, 20), b8(p0_prog_read, 12), b12(0));


    logic<32> imm;
    switch(b5(p0_prog_read, 2)) {
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

    reg_p1.hart = reg_p0.hart;
    reg_p1.pc = reg_p0.pc;
    reg_p1.inst = p0_prog_read;
    reg_p1.op = b5(p0_prog_read, 2);
    reg_p1.imm = imm;
    reg_p1.f3 = b3(p0_prog_read, 12);
    reg_p1.f7 = b7(p0_prog_read, 25);
    reg_p1.rd = b5(p0_prog_read, 7);

    sig_p01.hart  = reg_p0.hart;
    sig_p01.addr1 = b5(p0_prog_read, 15);
    sig_p01.addr2 = b5(p0_prog_read, 20);
  }

  //----------------------------------------


  static void delta12(const registers_p1& reg_p1, logic<32> p1_regs_read_data1, logic<32> p1_regs_read_data2, signals_p12& sig_p12, registers_p2& reg_p2) {
    logic<32> alu_in_a;
    logic<32> alu_in_b;
    logic<3> alu_op;
    logic<1> alu_alt;

    // Branch selection

    logic<1> eq  = p1_regs_read_data1 == p1_regs_read_data2;
    logic<1> slt = signed(p1_regs_read_data1) < signed(p1_regs_read_data2);
    logic<1> ult = p1_regs_read_data1 < p1_regs_read_data2;
    logic<1> take_branch;

    switch (reg_p1.f3) {
      case 0:  take_branch =   eq; break;
      case 1:  take_branch =  !eq; break;
      case 2:  take_branch =   eq; break;
      case 3:  take_branch =  !eq; break;
      case 4:  take_branch =  slt; break;
      case 5:  take_branch = !slt; break;
      case 6:  take_branch =  ult; break;
      case 7:  take_branch = !ult; break;
    }

    switch(reg_p1.op) {
      case OP_ALU:     reg_p2.pc_sel = 0; break;
      case OP_ALUI:    reg_p2.pc_sel = 0; break;
      case OP_LOAD:    reg_p2.pc_sel = 0; break;
      case OP_STORE:   reg_p2.pc_sel = 0; break;
      case OP_BRANCH:  reg_p2.pc_sel = take_branch; break;
      case OP_JAL:     reg_p2.pc_sel = 1; break;
      case OP_JALR:    reg_p2.pc_sel = 1; break;
      case OP_LUI:     reg_p2.pc_sel = 0; break;
      case OP_AUIPC:   reg_p2.pc_sel = 0; break;
    }

    // ALU

    switch(reg_p1.op) {
      case OP_ALU:     alu_in_a = p1_regs_read_data1; alu_in_b = p1_regs_read_data2; alu_op = reg_p1.f3; alu_alt = b1(reg_p1.f7, 5); break;
      case OP_ALUI:    alu_in_a = p1_regs_read_data1; alu_in_b = reg_p1.imm;         alu_op = reg_p1.f3; alu_alt = b1(reg_p1.f7, 5); break;
      case OP_LOAD:    alu_in_a = p1_regs_read_data1; alu_in_b = reg_p1.imm;         alu_op = 0; alu_alt = 0; break;
      case OP_STORE:   alu_in_a = p1_regs_read_data1; alu_in_b = reg_p1.imm;         alu_op = 0; alu_alt = 0; break;
      case OP_BRANCH:  alu_in_a = reg_p1.pc;          alu_in_b = reg_p1.imm;         alu_op = 0; alu_alt = 0; break;
      case OP_JAL:     alu_in_a = reg_p1.pc;          alu_in_b = reg_p1.imm;         alu_op = 0; alu_alt = 0; break;
      case OP_JALR:    alu_in_a = p1_regs_read_data1; alu_in_b = reg_p1.imm;         alu_op = 0; alu_alt = 0; break;
      case OP_LUI:     alu_in_a = 0;                  alu_in_b = reg_p1.imm;         alu_op = 0; alu_alt = 0; break;
      case OP_AUIPC:   alu_in_a = reg_p1.pc;          alu_in_b = reg_p1.imm;         alu_op = 0; alu_alt = 0; break;
    }

    logic<32> alu_result;
    switch (alu_op) {
      case 0: alu_result = (reg_p1.op == OP_ALU) && alu_alt ? alu_in_a - alu_in_b : alu_in_a + alu_in_b; break;
      case 1: alu_result = alu_in_a << b5(alu_in_b); break;
      case 2: alu_result = signed(alu_in_a) < signed(alu_in_b); break;
      case 3: alu_result = alu_in_a < alu_in_b; break;
      case 4: alu_result = alu_in_a ^ alu_in_b; break;
      case 5: alu_result = alu_alt ? sra(alu_in_a, b5(alu_in_b)) : sla(alu_in_a, b5(alu_in_b)); break;
      case 6: alu_result = alu_in_a | alu_in_b; break;
      case 7: alu_result = alu_in_a & alu_in_b; break;
    }

    reg_p2.hart = reg_p1.hart;
    reg_p2.alu = alu_result;
    reg_p2.rd = reg_p1.rd;
    reg_p2.op = reg_p1.op;
    reg_p2.f3 = reg_p1.f3;
    reg_p2.pc = reg_p1.pc;

    // Data mem read/write

    sig_p12.hart = reg_p1.hart;
    sig_p12.addr = alu_result;
    sig_p12.data    = p1_regs_read_data2 << (8 * b2(alu_result));
    if (reg_p1.op == OP_STORE) {
      sig_p12.byte_wren  = 0b1111;
      if (reg_p1.f3 == 0) sig_p12.byte_wren = 0b0001 << b2(alu_result);
      if (reg_p1.f3 == 1) sig_p12.byte_wren = 0b0011 << b2(alu_result);
    }
    else {
      sig_p12.byte_wren  = 0b0000;
    }
  }

  //----------------------------------------

  static void delta20(const registers_p2& reg_p2, const logic<32> p2_data_read, signals_p20& sig_p20, registers_p0& reg_p0) {
    logic<32> unpacked_data;
    switch (reg_p2.f3) {
      case 0: unpacked_data = sign_extend<32>(b8(p2_data_read, 8 * b2(reg_p2.alu))); break;
      case 1: unpacked_data = sign_extend<32>(b16(p2_data_read, 8 * b2(reg_p2.alu))); break;
      case 2: unpacked_data = p2_data_read; break;
      case 3: unpacked_data = p2_data_read; break;
      case 4: unpacked_data = b8(p2_data_read, 8 * b2(reg_p2.alu)); break;
      case 5: unpacked_data = b16(p2_data_read, 8 * b2(reg_p2.alu)); break;
      case 6: unpacked_data = p2_data_read; break;
      case 7: unpacked_data = p2_data_read; break;
    }

    switch(reg_p2.op) {
      case OP_ALU:     sig_p20.reg_wren = true;  sig_p20.reg_data = reg_p2.alu; break;
      case OP_ALUI:    sig_p20.reg_wren = true;  sig_p20.reg_data = reg_p2.alu; break;
      case OP_LOAD:    sig_p20.reg_wren = true;  sig_p20.reg_data = unpacked_data;     break;
      case OP_STORE:   sig_p20.reg_wren = false; sig_p20.reg_data = b32(DONTCARE);     break;
      case OP_BRANCH:  sig_p20.reg_wren = false; sig_p20.reg_data = b32(DONTCARE);     break;
      case OP_JAL:     sig_p20.reg_wren = true;  sig_p20.reg_data = reg_p2.pc + 4;     break;
      case OP_JALR:    sig_p20.reg_wren = true;  sig_p20.reg_data = reg_p2.pc + 4;     break;
      case OP_LUI:     sig_p20.reg_wren = true;  sig_p20.reg_data = reg_p2.alu; break;
      case OP_AUIPC:   sig_p20.reg_wren = true;  sig_p20.reg_data = reg_p2.alu; break;
    }

    logic<32> new_pc = reg_p2.pc_sel ? b32(reg_p2.alu) : b32(reg_p2.pc + 4);

    reg_p0.hart = reg_p2.hart;
    reg_p0.pc   = new_pc;

    sig_p20.hart     = reg_p2.hart;
    sig_p20.reg_addr = reg_p2.rd;
    sig_p20.mem_addr = new_pc;
  }

  //----------------------------------------

  void tick(logic<1> reset_in) {
    registers_p0 old_reg_p0 = reg_p0;
    registers_p1 old_reg_p1 = reg_p1;
    registers_p2 old_reg_p2 = reg_p2;

    signals_p20 old_sig_p20 = sig_p20;
    signals_p01 old_sig_p01 = sig_p01;
    signals_p12 old_sig_p12 = sig_p12;

    if (reset_in) {
      reset();
      return;
    }

    logic<32> old_data = data_mem[cat(old_sig_p12.hart, b10(old_sig_p12.addr, 2))];
    logic<32> old_prog = prog_mem[b10(old_sig_p20.mem_addr, 2)];

    if (old_sig_p12.byte_wren) {
      if (old_sig_p12.byte_wren[0]) old_data = (old_data & 0xFFFFFF00) | (old_sig_p12.data & 0x000000FF);
      if (old_sig_p12.byte_wren[1]) old_data = (old_data & 0xFFFF00FF) | (old_sig_p12.data & 0x0000FF00);
      if (old_sig_p12.byte_wren[2]) old_data = (old_data & 0xFF00FFFF) | (old_sig_p12.data & 0x00FF0000);
      if (old_sig_p12.byte_wren[3]) old_data = (old_data & 0x00FFFFFF) | (old_sig_p12.data & 0xFF000000);
      data_mem[cat(old_sig_p12.hart, b10(old_sig_p12.addr, 2))] = old_data;
    }

    if (old_sig_p20.reg_addr && old_sig_p20.reg_wren) {
      regfile[cat(old_sig_p20.hart, old_sig_p20.reg_addr)] = old_sig_p20.reg_data;
    }

    logic<32> old_regs_read_data1 = regfile[cat(old_sig_p01.hart, old_sig_p01.addr1)];
    logic<32> old_regs_read_data2 = regfile[cat(old_sig_p01.hart, old_sig_p01.addr2)];

    delta20(old_reg_p2, old_data, sig_p20, reg_p0);
    delta01(old_reg_p0, old_prog, sig_p01, reg_p1);
    delta12(old_reg_p1, old_regs_read_data1, old_regs_read_data2, sig_p12, reg_p2);
  }
};
