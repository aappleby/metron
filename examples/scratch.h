#ifndef PINWHEEL_RTL_PINWHEEL_CORE_H
#define PINWHEEL_RTL_PINWHEEL_CORE_H

#include "metron/metron_tools.h"
#include "pinwheel/tools/regfile_if.h"
#include "pinwheel/tools/tilelink.h"
#include "pinwheel/tools/riscv_constants.h"

//#include <assert.h>

// CSR 0x800 - swap secondary thread
// CSR 0x801 - yield primary thread

// 0xE0000000 - mapped regfile base address

// AB - Read registers
// BC - Execute n stuff
// CD - Memory read/write (todo)
// DA - Register writeback

// TODO - Move mem ops to CD so we can read code and still make the writeback

// FIXME we need to be able to stall an instruction so we can handle bus stalls
// on Tilelink

struct rv32_hpc {
  uint32_t active : 1;
  uint32_t hart : 7;
  uint32_t pc : 24;
};
//static_assert(sizeof(rv32_hpc) == 4);

//------------------------------------------------------------------------------
/* verilator lint_off UNUSEDSIGNAL */

class pinwheel_core {
public:

  pinwheel_core() {
    A_hpc.active = 0;
    A_hpc.hart = 0;
    A_hpc.pc = 0;

    B_hpc.active = 0;
    B_hpc.hart = 0;
    B_hpc.pc = 0;
    B_insn.raw = 0;

    C_hpc.active = 0;
    C_hpc.hart = 0;
    C_hpc.pc = 0;
    C_insn.raw = 0;
    C_addr = 0;
    C_result = 0;

    D_waddr = 0;
    D_wdata = 0;
    D_wren = 0;

    ticks = 0;

    code_tla.a_opcode  = TL::Invalid;
    code_tla.a_param   = b3(DONTCARE);
    code_tla.a_size    = b3(DONTCARE);
    code_tla.a_source  = b1(DONTCARE);
    code_tla.a_address = b32(DONTCARE);
    code_tla.a_mask    = b4(DONTCARE);
    code_tla.a_data    = b32(DONTCARE);
    code_tla.a_valid   = b1(DONTCARE);
    code_tla.a_ready   = b1(DONTCARE);

    data_tla.a_opcode  = TL::Invalid;
    data_tla.a_param   = b3(DONTCARE);
    data_tla.a_size    = b3(DONTCARE);
    data_tla.a_source  = b1(DONTCARE);
    data_tla.a_address = b32(DONTCARE);
    data_tla.a_mask    = b4(DONTCARE);
    data_tla.a_data    = b32(DONTCARE);
    data_tla.a_valid   = b1(DONTCARE);
    data_tla.a_ready   = b1(DONTCARE);
  }

  //----------------------------------------------------------------------------

  /* metron_noconvert */ logic<32> dbg_decode_imm(rv32_insn insn) const { return decode_imm(insn); }

  //----------------------------------------------------------------------------

  void tock(logic<1> reset_in, tilelink_d code_tld, tilelink_d data_tld, logic<32> _reg1, logic<32> _reg2) {
    rv32_insn AB_insn;
    AB_insn.raw = A_hpc.active ? code_tld.d_data : b32(0);

    //----------------------------------------
    // Forward stores from phase D to phase B

    logic<32> reg1 = _reg1;
    logic<32> reg2 = _reg2;

    if (D_waddr == cat(b7(B_hpc.hart), b5(B_insn.r.rs1)) && D_wren) reg1 = D_wdata;
    if (D_waddr == cat(b7(B_hpc.hart), b5(B_insn.r.rs2)) && D_wren) reg2 = D_wdata;

    logic<32> BC_reg1 = B_insn.r.rs1 ? reg1 : b32(0);
    logic<32> BC_reg2 = B_insn.r.rs2 ? reg2 : b32(0);
    logic<32> BC_imm  = decode_imm(B_insn);
    logic<32> BC_addr = b32(BC_reg1 + BC_imm);

    //----------------------------------------

    logic<1> BC_yield = B_hpc.active && B_insn.r.op == RV32I::OP_SYSTEM && B_insn.r.f3 == RV32I::F3_CSRRW && B_insn.c.csr == 0x801;
    logic<1> CD_yield = C_hpc.active && C_insn.r.op == RV32I::OP_SYSTEM && C_insn.r.f3 == RV32I::F3_CSRRW && C_insn.c.csr == 0x801;

    logic<1> BC_swap  = B_hpc.active && B_insn.r.op == RV32I::OP_SYSTEM && B_insn.r.f3 == RV32I::F3_CSRRW && B_insn.c.csr == 0x800;
    logic<1> CD_swap  = C_hpc.active && C_insn.r.op == RV32I::OP_SYSTEM && C_insn.r.f3 == RV32I::F3_CSRRW && C_insn.c.csr == 0x800;

    logic<1> BC_read_regfile  = B_hpc.active && B_insn.r.op == RV32I::OP_LOAD  && b4(BC_addr, 28) == 0xE;

    logic<1> CD_read_code     = C_hpc.active && C_insn.r.op == RV32I::OP_LOAD  && b4(C_addr, 28) == 0x0;
    logic<1> CD_read_regfile  = C_hpc.active && C_insn.r.op == RV32I::OP_LOAD  && b4(C_addr, 28) == 0xE;
    logic<1> CD_read_mem      = C_hpc.active && C_insn.r.op == RV32I::OP_LOAD  && b4(C_addr, 28) == 0x8;

    logic<1> CD_write_code    = C_hpc.active && C_insn.r.op == RV32I::OP_STORE && b4(C_addr, 28) == 0x0;
    logic<1> CD_write_mem     = C_hpc.active && C_insn.r.op == RV32I::OP_STORE && b4(C_addr, 28) == 0x8;
    logic<1> CD_write_regfile = C_hpc.active && C_insn.r.op == RV32I::OP_STORE && b4(C_addr, 28) == 0xE;

    //----------------------------------------
    // Vane B executes its instruction and stores the result in _result.

    rv32_hpc BC_hpc = next_hpc(B_hpc, B_insn, BC_reg1, BC_reg2);

    logic<32> BC_result = execute(B_hpc, B_insn, BC_reg1, BC_reg2, BC_hpc);

    //--------------------------------------------------------------------------
    // If both threads trigger PC swaps at once, the C one fires first.

    rv32_hpc BA_hpc = BC_hpc;

    if (CD_swap) {
      // If we're swapping threads, A is actually going to the HPC from reg1
      // that was passed in through C_result.
      BA_hpc = from_logic2(C_result);
    }
    else if (BC_yield) {
      // If we're yielding, A is actually going to the HPC in reg1.
      BA_hpc = from_logic2(BC_reg1);
    }
    else {
      BA_hpc = BC_hpc;
    }

    //--------------------------------------------------------------------------

    logic<32> CD_writeback;

    if      (CD_yield)        CD_writeback = C_result;

    // If we're switching secondary threads, we write the previous secondary
    // thread back to the primary thread's regfile.
    else if (CD_swap)         CD_writeback = to_logic(BC_hpc);

    // Note - this must be the _raw_ register, not zeroed, if we want to use
    // R0s in the regfile as spare storage
    else if (CD_read_regfile) CD_writeback = reg2;

    // A memory read replaces _result with the unpacked value on the data bus.
    else if (CD_read_mem)     CD_writeback = unpack_mem(C_insn.r.f3, C_addr, data_tld.d_data);
    else if (C_hpc.active)    CD_writeback = C_result;
    else                      CD_writeback = b32(DONTCARE);

    //--------------------------------------------------------------------------
    // Regfile write

    logic<13> CD_waddr;
    logic<32> CD_wdata;
    logic<1>  CD_wren;

    if (CD_write_regfile) {
      // A memory-mapped regfile write overrides the normal write.
      CD_waddr = b7(C_addr, 2);
      CD_wdata = CD_writeback;
      CD_wren  = 1;
    } else if (C_hpc.active) {
      // Vane C writes _result to _rd if the thread is active and _rd != 0.
      CD_waddr = cat(b7(C_hpc.hart), b5(C_insn.r.rd));
      CD_wdata = CD_writeback;
      CD_wren  = C_insn.r.rd && C_insn.r.op != RV32I::OP_STORE && C_insn.r.op != RV32I::OP_BRANCH;
    }
    else {
      CD_waddr = b12(DONTCARE);
      CD_wdata = b32(DONTCARE);
      CD_wren  = 0;
    }

    //--------------------------------------------------------------------------
    // Regfile read

    // If vane A is idle, vane B uses vane A's regfile slot to do an additional
    // regfile read. This is used for memory-mapped regfile reading.

    if (A_hpc.active) {
      reg_if.raddr1 = cat(b7(A_hpc.hart), b5(AB_insn.r.rs1));
      reg_if.raddr2 = cat(b7(A_hpc.hart), b5(AB_insn.r.rs2));
    }
    else if (BC_read_regfile) {
      reg_if.raddr1 = b12(DONTCARE);
      reg_if.raddr2 = b12(BC_addr, 2);
    }
    else {
      reg_if.raddr1 = b12(DONTCARE);
      reg_if.raddr2 = b12(DONTCARE);
    }

    //--------------------------------------------------------------------------
    // Code bus read/write


    // If the other thread is idle, vane C can override the code bus read to
    // write to code memory.
    if ((CD_read_code || CD_write_code) && !BA_hpc.active) {
      code_tla = gen_bus(C_insn.r.op, C_insn.r.f3, C_addr, C_result);
    }

    // Just load the next instruction for phase A.
    else {
      code_tla = gen_bus(RV32I::OP_LOAD, 2, BA_hpc.pc, C_result);
    }

    // FIXME move to phase C
    data_tla = gen_bus(B_insn.r.op, B_insn.r.f3, BC_addr, BC_reg2);

    reg_if.waddr  = D_waddr;
    reg_if.wdata  = D_wdata;
    reg_if.wren   = D_wren;

    tick(reset_in, AB_insn, BA_hpc, BC_addr, BC_result, CD_waddr, CD_wdata, CD_wren);
  }

  //----------------------------------------------------------------------------

private:

  static logic<32> execute(rv32_hpc old_hpc, rv32_insn insn, logic<32> reg1, logic<32> reg2, rv32_hpc next_hpc) {
    logic<1> yield = old_hpc.active && insn.r.op == RV32I::OP_SYSTEM && insn.r.f3 == RV32I::F3_CSRRW && insn.c.csr == 0x801;
    logic<1> swap  = old_hpc.active && insn.r.op == RV32I::OP_SYSTEM && insn.r.f3 == RV32I::F3_CSRRW && insn.c.csr == 0x800;
    logic<32> result;

    // If this thread is yielding, we store where it _would've_ gone in result
    // so we can write it to the regfile in phase C.
    if (yield) {
      result = to_logic(next_hpc);
    }

    // If we're going to swap secondary threads in phase C, our target hpc is
    // coming from reg1. We need to stash it in B_result so we can use it next
    // phase from C_result.
    else if (swap) {
      result = reg1;
    }

    // Regular instruction, execute it.
    else {
      logic<32> imm  = decode_imm(insn);
      logic<32> pc_plus_4 = old_hpc.pc + 4;
      switch(insn.r.op) {
        case RV32I::OP_OPIMM:  result = execute_alu(insn, reg1, imm); break;
        case RV32I::OP_OP:     result = execute_alu(insn, reg1, reg2); break;
        case RV32I::OP_SYSTEM: result = execute_system(old_hpc.hart, insn, reg1); break;
        case RV32I::OP_BRANCH: result = b32(DONTCARE); break;
        case RV32I::OP_JAL:    result = pc_plus_4; break;
        case RV32I::OP_JALR:   result = pc_plus_4; break;
        case RV32I::OP_LUI:    result = imm; break;
        case RV32I::OP_AUIPC:  result = old_hpc.pc + imm; break;
        case RV32I::OP_LOAD:   result = b32(DONTCARE); break;
        case RV32I::OP_STORE:  result = reg2; break;
        default:               result = b32(DONTCARE); break;
      }
    }

    return result;
  }

  //----------------------------------------------------------------------------

  static rv32_hpc next_hpc(rv32_hpc old_hpc, rv32_insn insn, logic<32> reg1, logic<32> reg2) {

    logic<32> imm  = decode_imm(insn);
    logic<32> pc_plus_4 = old_hpc.pc + 4;
    logic<32> pc_plus_imm = old_hpc.pc + imm;

    logic<1> new_take_branch = take_branch(insn.r.f3, reg1, reg2);

    logic<24> new_pc;
    switch(insn.r.op) {
      case RV32I::OP_BRANCH: new_pc = new_take_branch ? pc_plus_imm : pc_plus_4; break;
      case RV32I::OP_JAL:    new_pc = pc_plus_imm; break;
      case RV32I::OP_JALR:   new_pc = b24(reg1 + imm); break;
      case RV32I::OP_LUI:    new_pc = pc_plus_4; break;
      case RV32I::OP_AUIPC:  new_pc = pc_plus_4; break;
      case RV32I::OP_LOAD:   new_pc = pc_plus_4; break;
      case RV32I::OP_STORE:  new_pc = pc_plus_4; break;
      case RV32I::OP_SYSTEM: new_pc = pc_plus_4; break;
      case RV32I::OP_OPIMM:  new_pc = pc_plus_4; break;
      case RV32I::OP_OP:     new_pc = pc_plus_4; break;
      default:               new_pc = b24(DONTCARE); break;
    }

    rv32_hpc new_hpc;
    new_hpc.active = old_hpc.active;
    new_hpc.hart = old_hpc.hart;
    new_hpc.pc = new_pc;

    return new_hpc;
  }

  //----------------------------------------------------------------------------

  static logic<32> execute_system(logic<7> hart, rv32_insn insn, logic<32> B_reg1) {
    // FIXME need a good error if case is missing an expression

    logic<32> result = b32(DONTCARE);

    switch(insn.r.f3) {
      case RV32I::F3_CSRRW: {
        if (insn.c.csr == RV32I::MHARTID) result = b32(hart);
        break;
      }
      case RV32I::F3_CSRRS: {
        if (insn.c.csr == RV32I::MHARTID) result = b32(hart);
        break;
      }
    }

    return result;
  }

  //----------------------------------------------------------------------------
  // Translates a RV32I opcode into a TilelinkA transaction

  static tilelink_a gen_bus(logic<7> op, logic<3> f3, logic<32> addr, logic<32> reg2) {
    tilelink_a tla;

    logic<3> bus_size = b3(DONTCARE);
    logic<4> mask_b   = 0;

    if (f3 == 0) { mask_b = 0b0001; bus_size = 0; }
    if (f3 == 1) { mask_b = 0b0011; bus_size = 1; }
    if (f3 == 2) { mask_b = 0b1111; bus_size = 2; }
    if (addr[0]) mask_b = mask_b << 1;
    if (addr[1]) mask_b = mask_b << 2;

    tla.a_address = addr;
    tla.a_data    = (reg2 << ((addr & 3) * 8));
    tla.a_mask    = mask_b;
    tla.a_opcode  = (op == RV32I::OP_STORE) ? (bus_size == 2 ? TL::PutFullData : TL::PutPartialData) : TL::Get;
    tla.a_param   = b3(DONTCARE);
    tla.a_size    = bus_size;
    tla.a_source  = b1(DONTCARE);
    tla.a_valid   = (op == RV32I::OP_LOAD) || (op == RV32I::OP_STORE);
    tla.a_ready   = 1;

    return tla;
  }

  //----------------------------------------------------------------------------

  static logic<32> unpack_mem(logic<3> f3, logic<32> addr, logic<32> packed_mem) {
    logic<32> mem = packed_mem;

    if (addr[0]) mem = mem >> 8;
    if (addr[1]) mem = mem >> 16;

    switch (f3) {
      case RV32I::F3_LB:  mem = sign_extend<32>( b8(mem)); break;
      case RV32I::F3_LH:  mem = sign_extend<32>(b16(mem)); break;
      case RV32I::F3_LW:  mem = mem; break;
      case RV32I::F3_LD:  mem = mem; break;
      case RV32I::F3_LBU: mem = zero_extend<32>( b8(mem)); break;
      case RV32I::F3_LHU: mem = zero_extend<32>(b16(mem)); break;
      case RV32I::F3_LWU: mem = mem; break;
      case RV32I::F3_LDU: mem = mem; break;
    }

    return mem;
  }

  //----------------------------------------------------------------------------

  static logic<1> take_branch(logic<3> f3, logic<32> reg1, logic<32> reg2) {
    logic<1> eq  = reg1 == reg2;
    logic<1> slt = signed(reg1) < signed(reg2);
    logic<1> ult = reg1 < reg2;

    logic<1> result;
    switch (f3) {
      case RV32I::F3_BEQ:  result =   eq; break;
      case RV32I::F3_BNE:  result =  !eq; break;
      case RV32I::F3_BEQU: result =   eq; break;
      case RV32I::F3_BNEU: result =  !eq; break;
      case RV32I::F3_BLT:  result =  slt; break;
      case RV32I::F3_BGE:  result = !slt; break;
      case RV32I::F3_BLTU: result =  ult; break;
      case RV32I::F3_BGEU: result = !ult; break;
      default:             result = b1(DONTCARE); break;
    }

    return result;
  }

  //----------------------------------------------------------------------------

  static logic<32> decode_imm(rv32_insn insn2) {
    logic<32> imm_i = sign_extend<32>(b12(insn2.i.imm_11_0));
    logic<32> imm_s = sign_extend<32>(cat(b7(insn2.s.imm_11_5), b5(insn2.s.imm_4_0)));
    logic<32> imm_u = cat(b20(insn2.u.imm_31_12), b12(0));

    logic<32> imm_b = sign_extend<32>(cat(
      b1(insn2.b.imm_12),
      b1(insn2.b.imm_11),
      b6(insn2.b.imm_10_5),
      b4(insn2.b.imm_4_1),
      b1(0)
    ));

    logic<32> imm_j = sign_extend<32>(cat(
      b1 (insn2.j.imm_20),
      b8 (insn2.j.imm_19_12),
      b1 (insn2.j.imm_11),
      b10(insn2.j.imm_10_1),
      b1 (0)
    ));

    logic<32> result;

    switch(insn2.r.op) {
      case RV32I::OP_LOAD:   result = imm_i; break;
      case RV32I::OP_OPIMM:  result = imm_i; break;
      case RV32I::OP_AUIPC:  result = imm_u; break;
      case RV32I::OP_STORE:  result = imm_s; break;
      case RV32I::OP_OP:     result = imm_i; break;
      case RV32I::OP_LUI:    result = imm_u; break;
      case RV32I::OP_BRANCH: result = imm_b; break;
      case RV32I::OP_JALR:   result = imm_i; break;
      case RV32I::OP_JAL:    result = imm_j; break;
      default:               result = b32(DONTCARE); break;
    }

    return result;
  }

  //----------------------------------------------------------------------------

  static logic<32> execute_alu(rv32_insn insn, logic<32> alu1, logic<32> alu2) {
    logic<32> result;

    switch (insn.r.f3) {
      case RV32I::F3_ADDSUB: result = (insn.r.op == RV32I::OP_OP && insn.r.f7 == 32) ? alu1 - alu2 : alu1 + alu2; break;
      case RV32I::F3_SL:     result = alu1 << b5(alu2); break;
      case RV32I::F3_SLT:    result = signed(alu1) < signed(alu2); break;
      case RV32I::F3_SLTU:   result = alu1 < alu2; break;
      case RV32I::F3_XOR:    result = alu1 ^ alu2; break;
      case RV32I::F3_SR:     result = insn.r.f7 == 32 ? signed(alu1) >> b5(alu2) : alu1 >> b5(alu2); break;
      case RV32I::F3_OR:     result = alu1 | alu2; break;
      case RV32I::F3_AND:    result = alu1 & alu2; break;
      default:               result = b32(DONTCARE); break;
    }

    return result;
  }

  //----------------------------------------------------------------------------

  void tick(logic<1> reset_in,
            rv32_insn AB_insn,
            rv32_hpc BA_hpc,
            logic<32> BC_addr, logic<32> BC_result,
            logic<13> CD_waddr, logic<32> CD_wdata, logic<1> CD_wren)
  {
    if (reset_in) {
      A_hpc = from_logic1(1, 0, 0x00000004);

      B_hpc = from_logic1(0, 0, 0);
      B_insn.raw = 0;

      C_hpc = from_logic1(0, 0, 0);
      C_insn.raw = 0;
      C_addr     = 0;
      C_result   = 0;

      D_waddr = 0;
      D_wdata = 0;
      D_wren  = 0;

      ticks = 0x00000000;
    }
    else {
      D_waddr  = CD_waddr;
      D_wdata  = CD_wdata;
      D_wren   = CD_wren;

      C_hpc    = B_hpc;
      C_insn   = B_insn;
      C_addr   = BC_addr;
      C_result = BC_result;

      B_hpc    = A_hpc;
      B_insn   = AB_insn;

      A_hpc    = BA_hpc;

      ticks    = ticks + 1;
    }
  }

  static logic<32> to_logic(rv32_hpc hpc) {
    return cat(b1(hpc.active), b7(hpc.hart), b24(hpc.pc));
  }

  static rv32_hpc from_logic1(logic<1> active, logic<7> hart, logic<24> pc) {
    rv32_hpc result;
    result.active = active;
    result.hart = hart;
    result.pc = pc;
    return result;
  }

  static rv32_hpc from_logic2(logic<32> hpc) {
    rv32_hpc result;
    result.active = b1(hpc, 31);
    result.hart = b7(hpc, 24);
    result.pc = b24(hpc);
    return result;
  }

public:

  // FIXME yosys does not like this as a local variable
  tilelink_a data_tla;
  tilelink_a code_tla;
  regfile_if reg_if;

  //----------------------------------------
  // Internal signals and registers

  /* metron_internal */ rv32_hpc  A_hpc;

  /* metron_internal */ rv32_hpc  B_hpc;
  /* metron_internal */ rv32_insn B_insn;

  /* metron_internal */ rv32_hpc  C_hpc;
  /* metron_internal */ rv32_insn C_insn;
  /* metron_internal */ logic<32> C_addr;
  /* metron_internal */ logic<32> C_result;

  /* metron_internal */ logic<13> D_waddr;
  /* metron_internal */ logic<32> D_wdata;
  /* metron_internal */ logic<1>  D_wren;

  /* metron_internal */ logic<32> ticks;

};

/* verilator lint_on UNUSEDSIGNAL */
//------------------------------------------------------------------------------

#endif // PINWHEEL_RTL_PINWHEEL_CORE_H
