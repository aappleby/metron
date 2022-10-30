#pragma once
#include "metron_tools.h"

const int RV32I_OP_LOAD    = 0b00000;
const int RV32I_OP_LOADFP  = 0b00001;
const int RV32I_OP_CUSTOM0 = 0b00010;
const int RV32I_OP_MISCMEM = 0b00011;
const int RV32I_OP_OPIMM   = 0b00100;
const int RV32I_OP_AUIPC   = 0b00101;
const int RV32I_OP_OPIMM32 = 0b00110;
const int RV32I_OP_48B1    = 0b00111;

const int RV32I_OP_STORE   = 0b01000;
const int RV32I_OP_STOREFP = 0b01001;
const int RV32I_OP_CUSTOM1 = 0b01010;
const int RV32I_OP_AMO     = 0b01011;
const int RV32I_OP_OP      = 0b01100;
const int RV32I_OP_LUI     = 0b01101;
const int RV32I_OP_OP32    = 0b01110;
const int RV32I_OP_64B     = 0b01111;

const int RV32I_OP_MADD    = 0b10000;
const int RV32I_OP_MSUB    = 0b10001;
const int RV32I_OP_NMSUB   = 0b10010;
const int RV32I_OP_NMADD   = 0b10011;
const int RV32I_OP_OPFP    = 0b10100;
const int RV32I_OP_RES1    = 0b10101;
const int RV32I_OP_CUSTOM2 = 0b10110;
const int RV32I_OP_48B2    = 0b10111;

const int RV32I_OP_BRANCH  = 0b11000;
const int RV32I_OP_JALR    = 0b11001;
const int RV32I_OP_RES2    = 0b11010;
const int RV32I_OP_JAL     = 0b11011;
const int RV32I_OP_SYSTEM  = 0b11100;
const int RV32I_OP_RES3    = 0b11101;
const int RV32I_OP_CUSTOM3 = 0b11110;
const int RV32I_OP_80B     = 0b11111;

const int RV32I_F3_BEQ     = 0b000;
const int RV32I_F3_BNE     = 0b001;
const int RV32I_F3_BLT     = 0b100;
const int RV32I_F3_BGE     = 0b101;
const int RV32I_F3_BLTU    = 0b110;
const int RV32I_F3_BGEU    = 0b111;

const int RV32I_F3_LB      = 0b000;
const int RV32I_F3_LH      = 0b001;
const int RV32I_F3_LW      = 0b010;
const int RV32I_F3_LBU     = 0b100;
const int RV32I_F3_LHU     = 0b101;

const int RV32I_F3_SB      = 0b000;
const int RV32I_F3_SH      = 0b001;
const int RV32I_F3_SW      = 0b010;

const int RV32I_F3_ADDI = 0b000;
const int RV32I_F3_SLI     = 0b001;
const int RV32I_F3_SLTI    = 0b010;
const int RV32I_F3_SLTIU   = 0b011;
const int RV32I_F3_XORI    = 0b100;
const int RV32I_F3_SRI     = 0b101;
const int RV32I_F3_ORI     = 0b110;
const int RV32I_F3_ANDI    = 0b111;

const int RV32I_F3_ADDSUB  = 0b000;
const int RV32I_F3_SL      = 0b001;
const int RV32I_F3_SLT     = 0b010;
const int RV32I_F3_SLTU    = 0b011;
const int RV32I_F3_XOR     = 0b100;
const int RV32I_F3_SR      = 0b101;
const int RV32I_F3_OR      = 0b110;
const int RV32I_F3_AND     = 0b111;

const int RV32I_F3_CSRRW   = 0b001;
const int RV32I_F3_CSRRS   = 0b010;
const int RV32I_F3_CSRRC   = 0b011;
const int RV32I_F3_CSRRWI  = 0b101;
const int RV32I_F3_CSRRSI  = 0b110;
const int RV32I_F3_CSRRCI  = 0b111;

class Pinwheel {
 public:
  Pinwheel() {
    pc = 0;
    regs[0] = b32(0);

    std::string s;
    value_plusargs("text_file=%s", s);
    readmemh(s, text_mem);

    value_plusargs("data_file=%s", s);
    readmemh(s, data_mem);
  }

  void tock(logic<1> reset) {
    tick(reset);
  }

  logic<32> bus_read_data;
  logic<32> bus_address;
  logic<32> bus_write_data;
  logic<4> bus_byte_enable;
  logic<1> bus_read_enable;
  logic<1> bus_write_enable;
  logic<32> pc;

  //----------------------------------------

 private:
  static const int OP_ALU    = 0b00110011;
  static const int OP_ALUI   = 0b00010011;
  static const int OP_LOAD   = 0b00000011;
  static const int OP_STORE  = 0b00100011;
  static const int OP_BRANCH = 0b01100011;
  static const int OP_JAL    = 0b01101111;
  static const int OP_JALR   = 0b01100111;
  static const int OP_LUI    = 0b00110111;
  static const int OP_AUIPC  = 0b00010111;

  void do_reset() {
    pc = 0;
    regs[0] = b32(0);
    bus_read_data = 0;
    bus_address = 0;
    bus_write_data = 0;
    bus_byte_enable = 0;
    bus_read_enable = 0;
    bus_write_enable = 0;
  }

  void tick(logic<1> reset) {
    if (reset) {
      do_reset();
    } else {
      logic<32> inst = text_mem[b14(pc, 2)];

      logic<7> op = b7(inst, 0);
      logic<5> rd = b5(inst, 7);
      logic<3> f3 = b3(inst, 12);
      logic<5> r1 = b5(inst, 15);
      logic<5> r2 = b5(inst, 20);
      logic<7> f7 = b7(inst, 25);

      logic<32> imm_b = cat(dup<20>(inst[31]), inst[7], b6(inst, 25), b4(inst, 8), b1(0));
      logic<32> imm_i = cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20));
      logic<32> imm_j = cat(dup<12>(inst[31]), b8(inst, 12), inst[20], b6(inst, 25), b4(inst, 21), b1(0));
      logic<32> imm_s = cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 7));
      logic<32> imm_u = cat(inst[31], b11(inst, 20), b8(inst, 12), b12(0));

      bus_address = 0;
      bus_write_enable = 0;
      bus_write_data = 0;

      //----------
      // Metron simulates this a few percent faster if we don't have ALU and
      // ALUI in the same branch, but then we duplicate the big ALU switch...

      if (op == OP_ALU || op == OP_ALUI) {
        logic<32> op_a = regs[r1];
        logic<32> op_b =
            op == OP_ALUI ? cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20))
                          : regs[r2];
        logic<32> alu_result;

        // clang-format off
        switch (f3) {
          case 0: alu_result = (op == OP_ALU) && f7[5] ? op_a - op_b : op_a + op_b; break;
          case 1: alu_result = op_a << b5(op_b); break;
          case 2: alu_result = signed(op_a) < signed(op_b); break;
          case 3: alu_result = op_a < op_b; break;
          case 4: alu_result = op_a ^ op_b; break;
          case 5: {
            // FIXME BUG Verilator isn't handling this ternary expression
            // correctly.
            // alu_result = f7[5] ? sra(op_a, b5(op_b)) : b32(op_a >> b5(op_b));
            // break;
            if (f7[5]) {
              alu_result = sra(op_a, b5(op_b));
            } else {
              alu_result = op_a >> b5(op_b);
            }
            break;
          }
          case 6: alu_result = op_a | op_b; break;
          case 7: alu_result = op_a & op_b; break;
        }
        // clang-format on

        if (rd) regs[rd] = alu_result;
        pc = pc + 4;
      }

      //----------

      else if (op == OP_LOAD) {
        logic<32> imm = cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20));
        logic<32> addr = regs[r1] + imm;
        logic<32> data = data_mem[b15(addr, 2)] >> (8 * b2(addr));

        // clang-format off
        switch (f3) {
          case 0: data = sign_extend<32>(b8(data)); break;
          case 1: data = sign_extend<32>(b16(data)); break;
          case 4: data = b8(data); break;
          case 5: data = b16(data); break;
        }
        // clang-format on

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

        bus_address = addr;
        bus_write_enable = 1;
        bus_write_data = regs[r2];
      }

      //----------

      else if (op == OP_BRANCH) {
        logic<32> op_a = regs[r1];
        logic<32> op_b = regs[r2];

        // clang-format off
        logic<1> take_branch;
        switch (f3) {
          case 0: take_branch = op_a == op_b; break;
          case 1: take_branch = op_a != op_b; break;
          case 4: take_branch = signed(op_a) < signed(op_b); break;
          case 5: take_branch = signed(op_a) >= signed(op_b); break;
          case 6: take_branch = op_a < op_b; break;
          case 7: take_branch = op_a >= op_b; break;
          // KCOV_OFF
          default: take_branch = b1(DONTCARE); break;
          // KCOV_ON
        }
        // clang-format on

        if (take_branch) {
          logic<32> imm =
              cat(dup<20>(inst[31]), inst[7], b6(inst, 25), b4(inst, 8), b1(0));
          pc = pc + imm;
        } else {
          pc = pc + 4;
        }
      }

      //----------

      else if (op == OP_JAL) {
        logic<32> imm = cat(dup<12>(inst[31]), b8(inst, 12), inst[20],
                            b6(inst, 25), b4(inst, 21), b1(0));
        if (rd) regs[rd] = pc + 4;
        pc = pc + imm;
      }

      //----------

      else if (op == OP_JALR) {
        logic<32> rr1 = regs[r1];  // Lol, Metron actually found a bug - gotta
                                   // read r1 before writing
        logic<32> imm = cat(dup<21>(inst[31]), b6(inst, 25), b5(inst, 20));
        if (rd) regs[rd] = pc + 4;
        pc = rr1 + imm;
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
  }

  logic<32> text_mem[32 * 1024];
  logic<32> data_mem[32 * 1024];
  logic<32> regs[32];
};
