#pragma once
#include "metron_tools.h"
#include "constants.h"

class Pinwheel {
 public:
  void tock(logic<1> reset) {
    tick(reset);
  }

  logic<2> phase;
  logic<32> pc;
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

    data_write = 0;
    data_write_en = 0;
    data_byte_en = 0;
    regs_read_addr1 = 0;
    regs_read_addr2 = 0;
    regs_write_addr = 0;
    regs_write_data = 0;
    regs_write_en = 0;
    pc = 0;
    imm = 0;
    alu_result = 0;
    op = 0;
    f3 = 0;
    f7 = 0;
  }

  //----------------------------------------

  logic<32> imm;
  logic<5> op;
  logic<3> f3;
  logic<7> f7;
  logic<5>  regs_read_addr1;
  logic<5>  regs_read_addr2;

  void delta01(logic<32> prog_read) {
    logic<32> imm_b = cat(dup<20>(prog_read[31]), prog_read[7], b6(prog_read, 25), b4(prog_read, 8), b1(0));
    logic<32> imm_i = cat(dup<21>(prog_read[31]), b6(prog_read, 25), b5(prog_read, 20));
    logic<32> imm_j = cat(dup<12>(prog_read[31]), b8(prog_read, 12), prog_read[20], b6(prog_read, 25), b4(prog_read, 21), b1(0));
    logic<32> imm_s = cat(dup<21>(prog_read[31]), b6(prog_read, 25), b5(prog_read, 7));
    logic<32> imm_u = cat(prog_read[31], b11(prog_read, 20), b8(prog_read, 12), b12(0));

    op = b5(prog_read, 2);
    f3 = b3(prog_read, 12);
    f7 = b7(prog_read, 25);

    switch(op) {
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

    regs_read_addr1 = b5(prog_read, 15);
    regs_read_addr2 = b5(prog_read, 20);
    regs_write_addr = b5(prog_read, 7);
  }

  //----------------------------------------

  logic<1> pc_sel;

  void delta12(logic<32> imm, logic<32> regs_read_data1, logic<32> regs_read_data2) {
    logic<32> alu_in_a;
    logic<32> alu_in_b;
    logic<3> alu_op;
    logic<1> alu_alt;

    // Branch selection

    logic<1> eq  = regs_read_data1 == regs_read_data2;
    logic<1> slt = signed(regs_read_data1) < signed(regs_read_data2);
    logic<1> ult = regs_read_data1 < regs_read_data2;
    logic<1>  take_branch;

    switch (f3) {
      case 0:  take_branch =   eq; break;
      case 1:  take_branch =  !eq; break;
      case 2:  take_branch =   eq; break;
      case 3:  take_branch =  !eq; break;
      case 4:  take_branch =  slt; break;
      case 5:  take_branch = !slt; break;
      case 6:  take_branch =  ult; break;
      case 7:  take_branch = !ult; break;
    }

    switch(op) {
      case OP_ALU:     pc_sel = 0; break;
      case OP_ALUI:    pc_sel = 0; break;
      case OP_LOAD:    pc_sel = 0; break;
      case OP_STORE:   pc_sel = 0; break;
      case OP_BRANCH:  pc_sel = take_branch; break;
      case OP_JAL:     pc_sel = 1; break;
      case OP_JALR:    pc_sel = 1; break;
      case OP_LUI:     pc_sel = 0; break;
      case OP_AUIPC:   pc_sel = 0; break;
    }

    // ALU

    switch(op) {
      case OP_ALU:     alu_in_a = regs_read_data1; alu_in_b = regs_read_data2; alu_op = f3; alu_alt = b1(f7, 5); break;
      case OP_ALUI:    alu_in_a = regs_read_data1; alu_in_b = imm;             alu_op = f3; alu_alt = b1(f7, 5); break;
      case OP_LOAD:    alu_in_a = regs_read_data1; alu_in_b = imm;             alu_op = 0; alu_alt = 0; break;
      case OP_STORE:   alu_in_a = regs_read_data1; alu_in_b = imm;             alu_op = 0; alu_alt = 0; break;
      case OP_BRANCH:  alu_in_a = pc;              alu_in_b = imm;             alu_op = 0; alu_alt = 0; break;
      case OP_JAL:     alu_in_a = pc;              alu_in_b = imm;             alu_op = 0; alu_alt = 0; break;
      case OP_JALR:    alu_in_a = regs_read_data1; alu_in_b = imm;             alu_op = 0; alu_alt = 0; break;
      case OP_LUI:     alu_in_a = 0;               alu_in_b = imm;             alu_op = 0; alu_alt = 0;  break;
      case OP_AUIPC:   alu_in_a = pc;              alu_in_b = imm;             alu_op = 0; alu_alt = 0; break;
    }

    switch (alu_op) {
      case 0: alu_result = (op == OP_ALU) && alu_alt ? alu_in_a - alu_in_b : alu_in_a + alu_in_b; break;
      case 1: alu_result = alu_in_a << b5(alu_in_b); break;
      case 2: alu_result = signed(alu_in_a) < signed(alu_in_b); break;
      case 3: alu_result = alu_in_a < alu_in_b; break;
      case 4: alu_result = alu_in_a ^ alu_in_b; break;
      case 5: alu_result = alu_alt ? sra(alu_in_a, b5(alu_in_b)) : sla(alu_in_a, b5(alu_in_b)); break;
      case 6: alu_result = alu_in_a | alu_in_b; break;
      case 7: alu_result = alu_in_a & alu_in_b; break;
    }

    // Data mem read/write

    data_write    = regs_read_data2 << (8 * b2(alu_result));
    data_write_en = (op == OP_STORE);
    data_byte_en  = 0b1111;
    if (f3 == 0) data_byte_en = 0b0001 << b2(alu_result);
    if (f3 == 1) data_byte_en = 0b0011 << b2(alu_result);
  }

  //----------------------------------------

  logic<32> alu_result;
  logic<32> data_write;
  logic<1>  data_write_en;
  logic<4>  data_byte_en;

  //----------------------------------------

  void delta20(logic<32> data_read) {
    logic<32> unpacked_data;
    switch (f3) {
      case 0: unpacked_data = sign_extend<32>(b8(data_read, 8 * b2(alu_result))); break;
      case 1: unpacked_data = sign_extend<32>(b16(data_read, 8 * b2(alu_result))); break;
      case 2: unpacked_data = data_read; break;
      case 3: unpacked_data = data_read; break;
      case 4: unpacked_data = b8(data_read, 8 * b2(alu_result)); break;
      case 5: unpacked_data = b16(data_read, 8 * b2(alu_result)); break;
      case 6: unpacked_data = data_read; break;
      case 7: unpacked_data = data_read; break;
    }

    switch(op) {
      case OP_ALU:     regs_write_en = true;  regs_write_data = alu_result;    break;
      case OP_ALUI:    regs_write_en = true;  regs_write_data = alu_result;    break;
      case OP_LOAD:    regs_write_en = true;  regs_write_data = unpacked_data; break;
      case OP_STORE:   regs_write_en = false; regs_write_data = b32(DONTCARE); break;
      case OP_BRANCH:  regs_write_en = false; regs_write_data = b32(DONTCARE); break;
      case OP_JAL:     regs_write_en = true;  regs_write_data = pc + 4;        break;
      case OP_JALR:    regs_write_en = true;  regs_write_data = pc + 4;        break;
      case OP_LUI:     regs_write_en = true;  regs_write_data = alu_result;    break;
      case OP_AUIPC:   regs_write_en = true;  regs_write_data = alu_result;    break;
    }

    pc = pc_sel ? b32(alu_result) : b32(pc + 4);

    if (regs_write_addr && regs_write_en) {
      regs[regs_write_addr] = regs_write_data;
    }
  }

  //----------------------------------------

  logic<5>  regs_write_addr;
  logic<32> regs_write_data;
  logic<1>  regs_write_en;

  //----------------------------------------

  void tick(logic<1> reset_in) {
    logic<2> next_phase = 0;

    if (reset_in) {
      reset();
      phase = 0;
      return;
    }

    logic<32> prog_read = prog_mem[b14(pc, 2)];

    if (phase == 0) {
      delta01(prog_read);
      next_phase = 1;
    }

    logic<32> regs_read_data1 = regs[regs_read_addr1];
    logic<32> regs_read_data2 = regs[regs_read_addr2];

    if (phase == 1) {
      delta12(imm, regs_read_data1, regs_read_data2);
      next_phase = 2;
    }

    if (data_write_en) {
      logic<32> old_mem = data_mem[b15(alu_result, 2)];
      if (data_byte_en[0]) old_mem = (old_mem & 0xFFFFFF00) | (data_write & 0x000000FF);
      if (data_byte_en[1]) old_mem = (old_mem & 0xFFFF00FF) | (data_write & 0x0000FF00);
      if (data_byte_en[2]) old_mem = (old_mem & 0xFF00FFFF) | (data_write & 0x00FF0000);
      if (data_byte_en[3]) old_mem = (old_mem & 0x00FFFFFF) | (data_write & 0xFF000000);
      data_mem[b15(alu_result, 2)] = old_mem;
    }
    logic<32> data_read = data_mem[b15(alu_result, 2)];

    if (phase == 2) {
      delta20(data_read);
      next_phase = 0;
    }

    phase = next_phase;
  }
};
