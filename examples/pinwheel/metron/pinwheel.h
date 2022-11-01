#pragma once
#include "metron_tools.h"
#include "constants.h"

class Pinwheel {
 public:
  void tock(logic<1> reset) {
    tick(reset);
  }

  logic<2> phase;
  logic<32> regs[32];
  logic<32> prog_mem[32 * 1024];
  logic<32> data_mem[32 * 1024];

  //----------------------------------------

  static const int OP_ALU    = 0b01100;
  static const int OP_ALUI   = 0b00100;
  static const int OP_LOAD   = 0b00000;
  static const int OP_STORE  = 0b01000;
  static const int OP_BRANCH = 0b11000;
  static const int OP_JAL    = 0b11011;
  static const int OP_JALR   = 0b11001;
  static const int OP_LUI    = 0b01101;
  static const int OP_AUIPC  = 0b00101;

  //----------------------------------------

  void reset() {
    std::string s;
    value_plusargs("text_file=%s", s);
    readmemh(s, prog_mem);

    value_plusargs("data_file=%s", s);
    readmemh(s, data_mem);

    memset(regs, 0, sizeof(regs));

    p2_sig_data_data = 0;
    p2_sig_data_byte_wren = 0;
    p1_regs_write_addr = 0;
    p0_reg_pc = 0;
    p1_imm = 0;
    p2_reg_alu_result = 0;
    p1_op = 0;
    p1_f3 = 0;
    p1_f7 = 0;
  }

  //----------------------------------------

  void delta01(logic<32> p0_prog_read) {
    logic<32> imm_b = cat(dup<20>(p0_prog_read[31]), p0_prog_read[7], b6(p0_prog_read, 25), b4(p0_prog_read, 8), b1(0));
    logic<32> imm_i = cat(dup<21>(p0_prog_read[31]), b6(p0_prog_read, 25), b5(p0_prog_read, 20));
    logic<32> imm_j = cat(dup<12>(p0_prog_read[31]), b8(p0_prog_read, 12), p0_prog_read[20], b6(p0_prog_read, 25), b4(p0_prog_read, 21), b1(0));
    logic<32> imm_s = cat(dup<21>(p0_prog_read[31]), b6(p0_prog_read, 25), b5(p0_prog_read, 7));
    logic<32> imm_u = cat(p0_prog_read[31], b11(p0_prog_read, 20), b8(p0_prog_read, 12), b12(0));

    p1_pc = p0_reg_pc;
    p1_op = b5(p0_prog_read, 2);
    p1_f3 = b3(p0_prog_read, 12);
    p1_f7 = b7(p0_prog_read, 25);

    switch(p1_op) {
      case OP_ALU:    p1_imm = imm_i; break;
      case OP_ALUI:   p1_imm = imm_i; break;
      case OP_LOAD:   p1_imm = imm_i; break;
      case OP_STORE:  p1_imm = imm_s; break;
      case OP_BRANCH: p1_imm = imm_b; break;
      case OP_JAL:    p1_imm = imm_j; break;
      case OP_JALR:   p1_imm = imm_i; break;
      case OP_LUI:    p1_imm = imm_u; break;
      case OP_AUIPC:  p1_imm = imm_u; break;
    }

    p1_regs_read.addr1 = b5(p0_prog_read, 15);
    p1_regs_read.addr2 = b5(p0_prog_read, 20);
    p1_regs_write_addr = b5(p0_prog_read, 7);

  }

  //----------------------------------------

  struct sig_regs_read {
    logic<5> addr1;
    logic<5> addr2;
  };

  sig_regs_read p1_regs_read;

  logic<32> p1_pc;
  logic<32> p1_imm;
  logic<5>  p1_op;
  logic<3>  p1_f3;
  logic<7>  p1_f7;
  logic<5>  p1_regs_write_addr;
  logic<1>  p1_regs_write_en;

  //----------------------------------------

  struct sig_data {
    logic<5>  addr;
    logic<32> data;
    logic<4>  byte_wren;
  };

  void delta12(logic<32> p1_imm, logic<32> p1_regs_read_data1, logic<32> p1_regs_read_data2) {
    logic<32> alu_in_a;
    logic<32> alu_in_b;
    logic<3> alu_op;
    logic<1> alu_alt;

    // Branch selection

    logic<1> eq  = p1_regs_read_data1 == p1_regs_read_data2;
    logic<1> slt = signed(p1_regs_read_data1) < signed(p1_regs_read_data2);
    logic<1> ult = p1_regs_read_data1 < p1_regs_read_data2;
    logic<1> take_branch;

    switch (p1_f3) {
      case 0:  take_branch =   eq; break;
      case 1:  take_branch =  !eq; break;
      case 2:  take_branch =   eq; break;
      case 3:  take_branch =  !eq; break;
      case 4:  take_branch =  slt; break;
      case 5:  take_branch = !slt; break;
      case 6:  take_branch =  ult; break;
      case 7:  take_branch = !ult; break;
    }

    switch(p1_op) {
      case OP_ALU:     p2_reg_pc_sel = 0; break;
      case OP_ALUI:    p2_reg_pc_sel = 0; break;
      case OP_LOAD:    p2_reg_pc_sel = 0; break;
      case OP_STORE:   p2_reg_pc_sel = 0; break;
      case OP_BRANCH:  p2_reg_pc_sel = take_branch; break;
      case OP_JAL:     p2_reg_pc_sel = 1; break;
      case OP_JALR:    p2_reg_pc_sel = 1; break;
      case OP_LUI:     p2_reg_pc_sel = 0; break;
      case OP_AUIPC:   p2_reg_pc_sel = 0; break;
    }

    // ALU

    switch(p1_op) {
      case OP_ALU:     alu_in_a = p1_regs_read_data1; alu_in_b = p1_regs_read_data2; alu_op = p1_f3; alu_alt = b1(p1_f7, 5); break;
      case OP_ALUI:    alu_in_a = p1_regs_read_data1; alu_in_b = p1_imm;             alu_op = p1_f3; alu_alt = b1(p1_f7, 5); break;
      case OP_LOAD:    alu_in_a = p1_regs_read_data1; alu_in_b = p1_imm;             alu_op = 0; alu_alt = 0; break;
      case OP_STORE:   alu_in_a = p1_regs_read_data1; alu_in_b = p1_imm;             alu_op = 0; alu_alt = 0; break;
      case OP_BRANCH:  alu_in_a = p0_reg_pc;          alu_in_b = p1_imm;             alu_op = 0; alu_alt = 0; break;
      case OP_JAL:     alu_in_a = p0_reg_pc;          alu_in_b = p1_imm;             alu_op = 0; alu_alt = 0; break;
      case OP_JALR:    alu_in_a = p1_regs_read_data1; alu_in_b = p1_imm;             alu_op = 0; alu_alt = 0; break;
      case OP_LUI:     alu_in_a = 0;                  alu_in_b = p1_imm;             alu_op = 0; alu_alt = 0; break;
      case OP_AUIPC:   alu_in_a = p0_reg_pc;          alu_in_b = p1_imm;             alu_op = 0; alu_alt = 0; break;
    }

    switch (alu_op) {
      case 0: p2_reg_alu_result = (p1_op == OP_ALU) && alu_alt ? alu_in_a - alu_in_b : alu_in_a + alu_in_b; break;
      case 1: p2_reg_alu_result = alu_in_a << b5(alu_in_b); break;
      case 2: p2_reg_alu_result = signed(alu_in_a) < signed(alu_in_b); break;
      case 3: p2_reg_alu_result = alu_in_a < alu_in_b; break;
      case 4: p2_reg_alu_result = alu_in_a ^ alu_in_b; break;
      case 5: p2_reg_alu_result = alu_alt ? sra(alu_in_a, b5(alu_in_b)) : sla(alu_in_a, b5(alu_in_b)); break;
      case 6: p2_reg_alu_result = alu_in_a | alu_in_b; break;
      case 7: p2_reg_alu_result = alu_in_a & alu_in_b; break;
    }

    // Data mem read/write

    p2_sig_data_addr = p2_reg_alu_result;
    p2_sig_data_data    = p1_regs_read_data2 << (8 * b2(p2_reg_alu_result));
    if (p1_op == OP_STORE) {
      p2_sig_data_byte_wren  = 0b1111;
      if (p1_f3 == 0) p2_sig_data_byte_wren = 0b0001 << b2(p2_reg_alu_result);
      if (p1_f3 == 1) p2_sig_data_byte_wren = 0b0011 << b2(p2_reg_alu_result);
    }
    else {
      p2_sig_data_byte_wren  = 0b0000;
    }

    p2_reg_regs_write_addr = p1_regs_write_addr;
    p2_op = p1_op;
    p2_f3 = p1_f3;
    p2_reg_pc = p1_pc;
  }

  //----------------------------------------

  logic<32> p2_reg_pc;
  logic<5>  p2_op;
  logic<3>  p2_f3;
  logic<1>  p2_reg_pc_sel;
  logic<32> p2_reg_alu_result;

  sig_data p2_sig_data;

  logic<32> p2_sig_data_addr;
  logic<32> p2_sig_data_data;
  logic<4>  p2_sig_data_byte_wren;

  logic<5>  p2_reg_regs_write_addr;

  //----------------------------------------

  struct sig_regs_write {
    logic<5>  addr;
    logic<32> data;
    logic<1>  en;
  };

  sig_regs_write delta20(logic<32> p2_data_read, logic<32> p2_reg_alu_result) {
    logic<32> unpacked_data;
    switch (p2_f3) {
      case 0: unpacked_data = sign_extend<32>(b8(p2_data_read, 8 * b2(p2_reg_alu_result))); break;
      case 1: unpacked_data = sign_extend<32>(b16(p2_data_read, 8 * b2(p2_reg_alu_result))); break;
      case 2: unpacked_data = p2_data_read; break;
      case 3: unpacked_data = p2_data_read; break;
      case 4: unpacked_data = b8(p2_data_read, 8 * b2(p2_reg_alu_result)); break;
      case 5: unpacked_data = b16(p2_data_read, 8 * b2(p2_reg_alu_result)); break;
      case 6: unpacked_data = p2_data_read; break;
      case 7: unpacked_data = p2_data_read; break;
    }

    sig_regs_write p0_sig_regs_write;
    switch(p2_op) {
      case OP_ALU:     p0_sig_regs_write.en = true;  p0_sig_regs_write.data = p2_reg_alu_result;    break;
      case OP_ALUI:    p0_sig_regs_write.en = true;  p0_sig_regs_write.data = p2_reg_alu_result;    break;
      case OP_LOAD:    p0_sig_regs_write.en = true;  p0_sig_regs_write.data = unpacked_data; break;
      case OP_STORE:   p0_sig_regs_write.en = false; p0_sig_regs_write.data = b32(DONTCARE); break;
      case OP_BRANCH:  p0_sig_regs_write.en = false; p0_sig_regs_write.data = b32(DONTCARE); break;
      case OP_JAL:     p0_sig_regs_write.en = true;  p0_sig_regs_write.data = p2_reg_pc + 4;        break;
      case OP_JALR:    p0_sig_regs_write.en = true;  p0_sig_regs_write.data = p2_reg_pc + 4;        break;
      case OP_LUI:     p0_sig_regs_write.en = true;  p0_sig_regs_write.data = p2_reg_alu_result;    break;
      case OP_AUIPC:   p0_sig_regs_write.en = true;  p0_sig_regs_write.data = p2_reg_alu_result;    break;
    }

    p0_reg_pc = p2_reg_pc_sel ? b32(p2_reg_alu_result) : b32(p2_reg_pc + 4);
    p0_sig_regs_write.addr = p2_reg_regs_write_addr;
    return p0_sig_regs_write;
  }

  //----------------------------------------

  logic<32> p0_reg_pc;


  //----------------------------------------

  void tick(logic<1> reset_in) {
    logic<2> next_phase = 0;

    if (reset_in) {
      reset();
      phase = 0;
      return;
    }

    logic<32> prog_read = prog_mem[b14(p0_reg_pc, 2)];

    if (phase == 0) {
      delta01(prog_read);
      next_phase = 1;
    }

    logic<32> regs_read_data1 = regs[p1_regs_read.addr1];
    logic<32> regs_read_data2 = regs[p1_regs_read.addr2];

    if (phase == 1) {
      delta12(p1_imm, regs_read_data1, regs_read_data2);
      next_phase = 2;
    }

    if (p2_sig_data_byte_wren) {
      logic<32> old_mem = data_mem[b15(p2_reg_alu_result, 2)];
      if (p2_sig_data_byte_wren[0]) old_mem = (old_mem & 0xFFFFFF00) | (p2_sig_data_data & 0x000000FF);
      if (p2_sig_data_byte_wren[1]) old_mem = (old_mem & 0xFFFF00FF) | (p2_sig_data_data & 0x0000FF00);
      if (p2_sig_data_byte_wren[2]) old_mem = (old_mem & 0xFF00FFFF) | (p2_sig_data_data & 0x00FF0000);
      if (p2_sig_data_byte_wren[3]) old_mem = (old_mem & 0x00FFFFFF) | (p2_sig_data_data & 0xFF000000);
      data_mem[b15(p2_reg_alu_result, 2)] = old_mem;
    }
    logic<32> data_read = data_mem[b15(p2_reg_alu_result, 2)];

    if (phase == 2) {
      sig_regs_write p0_sig_regs_write = delta20(data_read, p2_reg_alu_result);

      if (p0_sig_regs_write.addr && p0_sig_regs_write.en) {
        regs[p0_sig_regs_write.addr] = p0_sig_regs_write.data;
      }

      next_phase = 0;
    }

    phase = next_phase;
  }
};
