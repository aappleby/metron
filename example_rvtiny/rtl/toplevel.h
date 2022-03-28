#ifndef RVTINY_TOPLEVEL_H
#define RVTINY_TOPLEVEL_H

#include "metron_tools.h"

class toplevel {
 public:

  logic<32> o_bus_read_data;
  logic<32> o_bus_address;
  logic<32> o_bus_write_data;
  logic<4>  o_bus_byte_enable;
  logic<1>  o_bus_read_enable;
  logic<1>  o_bus_write_enable;
  logic<32> o_inst;
  logic<32> o_pc;

  const int OP_ALU    = 0x33;
  const int OP_ALUI   = 0x13;
  const int OP_LOAD   = 0x03;
  const int OP_STORE  = 0x23;
  const int OP_BRANCH = 0x63;
  const int OP_JAL    = 0x6F;
  const int OP_JALR   = 0x67;
  const int OP_LUI    = 0x37;
  const int OP_AUIPC  = 0x17;

  void init() {
    pc = 0;
    regs[0] = b32(0);

    std::string s;
    value_plusargs("text_file=%s", s);
    readmemh(s, text_mem);

    value_plusargs("data_file=%s", s);
    readmemh(s, data_mem);
  }

  void tock(logic<1> reset) {
    if (reset) {
      pc = 0;
      regs[0] = b32(0);
      o_bus_read_data = 0;
      o_bus_address = 0;
      o_bus_write_data = 0;
      o_bus_byte_enable = 0;
      o_bus_read_enable = 0;
      o_bus_write_enable = 0;
      o_inst = 0;
      o_pc = 0;
      return;
    }

    logic<32> inst = text_mem[b14(pc, 2)];

    logic<7> op = b7(inst, 0);
    logic<5> rd = b5(inst, 7);
    logic<3> f3 = b3(inst, 12);
    logic<5> r1 = b5(inst, 15);
    logic<5> r2 = b5(inst, 20);
    logic<7> f7 = b7(inst, 25);

    o_bus_address = 0;
    o_bus_write_enable = 0;
    o_bus_write_data = 0;

    //----------
    // Metron simulates this a few percent faster if we don't have ALU and ALUI in the same branch,
    // but then we duplicate the big ALU switch...

    if (op == OP_ALU || op == OP_ALUI) {
      logic<32> op_a = regs[r1];
      logic<32> op_b = op == OP_ALUI ? cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20)) : regs[r2];
      logic<32> alu_result;

      switch(f3) {
      case 0: alu_result = (op == OP_ALU) && f7[5] ? op_a - op_b : op_a + op_b; break;
      case 1: alu_result = op_a << b5(op_b); break;
      case 2: alu_result = signed(op_a) < signed(op_b); break;
      case 3: alu_result = unsigned(op_a) < unsigned(op_b); break;
      case 4: alu_result = op_a ^ op_b; break;
      case 5: alu_result = f7[5] ? sra(op_a, b5(op_b)) : logic<32>(op_a >> b5(op_b)); break;
      case 6: alu_result = op_a | op_b; break;
      case 7: alu_result = op_a & op_b; break;
      }

      if (rd) regs[rd] = alu_result;
      pc = pc + 4;
    }

    //----------

    else if (op == OP_LOAD) {
      logic<32> imm = cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20));
      logic<32> addr = regs[r1] + imm;
      logic<32> data = data_mem[b15(addr, 2)] >> (8 * b2(addr));

      switch (f3) {
      case 0:  data = int8_t(data);   break;
      case 1:  data = int16_t(data);  break;
      case 2:  data = int32_t(data);  break;
      case 3:  data = int64_t(data);  break;
      case 4:  data = uint8_t(data);  break;
      case 5:  data = uint16_t(data); break;
      case 6:  data = uint32_t(data); break;
      case 7:  data = uint64_t(data); break;
      }

      if (rd) regs[rd] = data;
      pc = pc + 4;
    }

    //----------

    else if (op == OP_STORE) {
      logic<32> imm = cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 7));
      logic<32> addr = regs[r1] + imm;
      logic<32> data = regs[r2] << (8 * b2(addr));

      logic<32> mask = 0;
      if (f3 == 0) mask = 0x000000FF << (8 * b2(addr));
      if (f3 == 1) mask = 0x0000FFFF << (8 * b2(addr));
      if (f3 == 2) mask = 0xFFFFFFFF;

      logic<15> phys_addr = b15(addr, 2);
      data_mem[phys_addr] = (data_mem[phys_addr] & ~mask) | (data & mask);

      pc = pc + 4;

      o_bus_address = addr;
      o_bus_write_enable = 1;
      o_bus_write_data = regs[r2];
    }

    //----------

    else if (op == OP_BRANCH) {
      logic<32> op_a = regs[r1];
      logic<32> op_b = regs[r2];

      logic<1> take_branch;
      switch (f3) {
      case 0:  take_branch = op_a == op_b; break;
      case 1:  take_branch = op_a != op_b; break;
      case 4:  take_branch = signed(op_a) <  signed(op_b); break;
      case 5:  take_branch = signed(op_a) >= signed(op_b); break;
      case 6:  take_branch = unsigned(op_a) < unsigned(op_b); break;
      case 7:  take_branch = unsigned(op_a) >= unsigned(op_b); break;
      default: take_branch = b1(DONTCARE); break;
      }

      if (take_branch) {
        logic<32> imm = cat(dup<20>(inst[31]), inst[7], b6(inst, 25), b4(inst, 8), b1(0));
        pc = pc + imm;
      } else {
        pc = pc + 4;
      }
    }

    //----------

    else if (op == OP_JAL) {
      logic<32> imm = cat(dup<12>(inst[31]), b8(inst, 12), inst[20], b6(inst, 25), b4(inst, 21), b1(0));
      if (rd) regs[rd] = pc + 4;
      pc = pc + imm;
    }

    //----------

    else if (op == OP_JALR) {
      logic<32> imm = cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20));
      if (rd) regs[rd] = pc + 4;
      pc = regs[r1] + imm;
    }

    //----------

    else if (op == OP_LUI) {
      logic<32> imm = cat(inst[31], b11(inst, 20), b8(inst, 12), b12(0));
      if (rd) regs[rd] = imm;
      pc = pc + 4;
    }

    //----------

    else if (op == OP_AUIPC) {
      logic<32> imm = cat(inst[31], b11(inst, 20), b8(inst, 12), b12(0));
      if (rd) regs[rd] = pc + imm;
      pc = pc + 4;
    }
  }

  //----------------------------------------

private:
  logic<32> text_mem[0x10000 >> 2];
  logic<32> data_mem[0x20000 >> 2];
  logic<32> pc;
  logic<32> regs[32];
};


#endif  // RVTINY_TOPLEVEL_H
