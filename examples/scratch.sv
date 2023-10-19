`ifndef PINWHEEL_CORE_H
`define PINWHEEL_CORE_H

`include "metron/metron_tools.sv"
`include "pinwheel/rtl/regfile.sv"
//#include "pinwheel/rtl/tilelink.h"
//#include "pinwheel/tools/riscv_constants.h"

/* verilator lint_off UNUSEDSIGNAL */

typedef struct packed {
  logic[7:0]  raddr1;
  logic[7:0]  raddr2;
  logic[7:0]  waddr;
  logic[31:0] wdata;
  logic  wren;
} regfile_in;


module pinwheel_core (
);
/*public:*/

<<<<<<< Updated upstream
  always_comb begin : func
    /*const*/ int thing2;
    parameter /*static*/ /*const*/ int thing1 = 1;
    thing2 = 2;
    func_ret = thing1 + thing2;
=======
/*

  //----------------------------------------

  void tock(logic<1> reset_in, tilelink_d code_tld, tilelink_d bus_tld, logic<32> reg_rdata1, logic<32> reg_rdata2) {

    sig_insn_a  = b24(reg_hpc_a) ? code_tld.d_data : b32(0);
    logic<5>  rs1a_a  = b5(sig_insn_a, 15);
    logic<5>  rs2a_a  = b5(sig_insn_a, 20);
    sig_rf_raddr1 = cat(b3(reg_hpc_a, 24), rs1a_a);
    sig_rf_raddr2 = cat(b3(reg_hpc_a, 24), rs2a_a);

    logic<5>  op_b   = b5(reg_insn_b, 2);
    logic<3>  f3_b   = b3(reg_insn_b, 12);
    logic<5>  rs1a_b = b5(reg_insn_b, 15);
    logic<5>  rs2a_b = b5(reg_insn_b, 20);
    logic<32> rs1_b  = rs1a_b ? reg_rdata1 : b32(0);
    logic<32> rs2_b  = rs2a_b ? reg_rdata2 : b32(0);
    logic<32> imm_b  = decode_imm(reg_insn_b);
    sig_addr_b  = b32(rs1_b + imm_b);
    logic<1>  regfile_cs_b = b4(sig_addr_b, 28) == 0xE;
    logic<12> csr_b = b12(reg_insn_b, 20);

    logic<5>  op_c  = b5(reg_insn_c, 2);
    logic<5>  rd_c  = b5(reg_insn_c, 7);
    logic<3>  f3_c  = b3(reg_insn_c, 12);
    logic<12> csr_c = b12(reg_insn_c, 20);
    logic<4>  bus_tag_c    = b4(reg_addr_c, 28);
    logic<1>  regfile_cs_c = bus_tag_c == 0xE;
    logic<32> data_out_c   = regfile_cs_c ? reg_rdata1 : bus_tld.d_data;

    logic<32> temp_result_c = reg_result_c;

    //----------
    // Fetch

    logic<32> next_hpc = 0;
    {
      logic<1> take_branch = 0;
      if (b24(reg_hpc_b)) {
        logic<1> eq  = rs1_b == rs2_b;
        logic<1> slt = signed(rs1_b) < signed(rs2_b);
        logic<1> ult = rs1_b < rs2_b;

        switch (f3_b) {
          case 0:  take_branch =   eq; break;
          case 1:  take_branch =  !eq; break;
          case 2:  take_branch =   eq; break;
          case 3:  take_branch =  !eq; break;
          case 4:  take_branch =  slt; break;
          case 5:  take_branch = !slt; break;
          case 6:  take_branch =  ult; break;
          case 7:  take_branch = !ult; break;
          default: take_branch =    0; break;
        }
      }

      if (b24(reg_hpc_b)) {
        switch(op_b) {
          case RV32I::OP_BRANCH: next_hpc = take_branch ? reg_hpc_b + imm_b : reg_hpc_b + 4; break;
          case RV32I::OP_JAL:    next_hpc = reg_hpc_b + imm_b; break;
          case RV32I::OP_JALR:   next_hpc = sig_addr_b; break;
          case RV32I::OP_LUI:    next_hpc = reg_hpc_b + 4; break;
          case RV32I::OP_AUIPC:  next_hpc = reg_hpc_b + 4; break;
          case RV32I::OP_LOAD:   next_hpc = reg_hpc_b + 4; break;
          case RV32I::OP_STORE:  next_hpc = reg_hpc_b + 4; break;
          case RV32I::OP_SYSTEM: next_hpc = reg_hpc_b + 4; break;
          case RV32I::OP_OPIMM:  next_hpc = reg_hpc_b + 4; break;
          case RV32I::OP_OP:     next_hpc = reg_hpc_b + 4; break;
        }
      }
    }

    //----------
    // Execute

    logic<32> alu_result = 0;
    {
      switch(op_b) {
        case RV32I::OP_BRANCH: alu_result = b32(DONTCARE); break;
        case RV32I::OP_JAL:    alu_result = reg_hpc_b + 4;     break;
        case RV32I::OP_JALR:   alu_result = reg_hpc_b + 4;     break;
        case RV32I::OP_LUI:    alu_result = imm_b;         break;
        case RV32I::OP_AUIPC:  alu_result = reg_hpc_b + imm_b; break;
        case RV32I::OP_LOAD:   alu_result = sig_addr_b;     break;
        case RV32I::OP_STORE:  alu_result = rs2_b;         break;
        case RV32I::OP_SYSTEM: alu_result = execute_system(reg_insn_b, rs1_b, rs2_b); break;
        case RV32I::OP_OPIMM:  alu_result = execute_alu   (reg_insn_b, rs1_b, rs2_b); break;
        case RV32I::OP_OP:     alu_result = execute_alu   (reg_insn_b, rs1_b, rs2_b); break;
        default:               alu_result = b32(DONTCARE); break;
      }

      if (op_b == RV32I::OP_SYSTEM && f3_b == RV32I::F3_CSRRW && csr_b == 0x801) {
        logic<32> temp = alu_result;
        alu_result = next_hpc;
        next_hpc = temp;
      }

      if (op_c == RV32I::OP_SYSTEM && f3_c == RV32I::F3_CSRRW && csr_c == 0x800) {
        logic<32> temp = temp_result_c;
        temp_result_c = next_hpc;
        next_hpc = temp;
      }

    }
    sig_hpc_a = next_hpc;
    sig_result_b = alu_result;

    //----------
    // Memory: Data bus

    {
      logic<4>          temp_mask_b = 0;
      if (f3_b == 0)    temp_mask_b = 0b0001;
      if (f3_b == 1)    temp_mask_b = 0b0011;
      if (f3_b == 2)    temp_mask_b = 0b1111;
      if (sig_addr_b[0]) temp_mask_b = temp_mask_b << 1;
      if (sig_addr_b[1]) temp_mask_b = temp_mask_b << 2;

      sig_bus_addr   = sig_addr_b;
      sig_bus_rden   = (op_b == RV32I::OP_LOAD);
      sig_bus_wdata  = rs2_b;
      sig_bus_wmask  = temp_mask_b;
      sig_bus_wren   = (op_b == RV32I::OP_STORE);
    }

    //----------
    // Memory + code/data/reg read/write overrides for cross-thread stuff

    {
      // We write code memory in phase C because it's busy reading the next
      // instruction in phase B.

      // Hmm we can't actually read from code because we also have to read our next instruction
      // and we can't do it earlier or later (we can read it during C, but then it's not back
      // in time to write to the regfile).

      logic<4>       temp_mask_c = 0;
      if (f3_c == 0) temp_mask_c = 0b0001;
      if (f3_c == 1) temp_mask_c = 0b0011;
      if (f3_c == 2) temp_mask_c = 0b1111;
      if (reg_addr_c[0]) temp_mask_c = temp_mask_c << 1;
      if (reg_addr_c[1]) temp_mask_c = temp_mask_c << 2;

      logic<1> code_cs_c = bus_tag_c == 0x0 && b24(sig_hpc_a) == 0;

      sig_code_addr  = code_cs_c ? b24(reg_addr_c) : b24(sig_hpc_a);
      sig_code_wdata = temp_result_c;
      sig_code_wmask = temp_mask_c;
      sig_code_wren  = (op_c == RV32I::OP_STORE) && code_cs_c;
    }

    //----------
    // Regfile write

    {
      logic<32> unpacked_c = data_out_c;
      if (temp_result_c[0]) unpacked_c = unpacked_c >> 8;
      if (temp_result_c[1]) unpacked_c = unpacked_c >> 16;
      switch (f3_c) {
        case 0:  unpacked_c = sign_extend<32>( b8(unpacked_c)); break;
        case 1:  unpacked_c = sign_extend<32>(b16(unpacked_c)); break;
        case 4:  unpacked_c = zero_extend<32>( b8(unpacked_c)); break;
        case 5:  unpacked_c = zero_extend<32>(b16(unpacked_c)); break;
      }

      // If we're using jalr to jump between threads, we use the hart from HPC _A_
      // as the target for the write so that the link register will be written
      // in the _destination_ regfile.

      sig_rf_waddr = cat(b3(op_c == RV32I::OP_JALR ? reg_hpc_a : reg_hpc_c, 24), rd_c);
      sig_rf_wdata = op_c == RV32I::OP_LOAD ? unpacked_c : temp_result_c;
      sig_rf_wren  = b24(reg_hpc_c) && op_c != RV32I::OP_STORE && op_c != RV32I::OP_BRANCH;

      if (rd_c == 0) sig_rf_wren = 0;

      if ((op_b == RV32I::OP_LOAD) && regfile_cs_b && (b24(reg_hpc_a) == 0)) {
        sig_rf_raddr1 = b10(sig_addr_b >> 2);
      }

      // Handle stores through the bus to the regfile.
      if (op_c == RV32I::OP_STORE && regfile_cs_c) {
        sig_rf_waddr = b10(reg_addr_c >> 2);
        sig_rf_wdata = temp_result_c;
        sig_rf_wren = 1;
      }
    }

    sig_result_c = temp_result_c;

    bus_tla.a_opcode  = sig_bus_wren ? TL::PutPartialData : TL::Get;
    bus_tla.a_param   = b3(DONTCARE);
    bus_tla.a_size    = 0; // fixme
    bus_tla.a_source  = b1(DONTCARE);
    bus_tla.a_address = sig_bus_addr;
    bus_tla.a_mask    = sig_bus_wmask;
    bus_tla.a_data    = sig_bus_wdata;
    bus_tla.a_valid   = 1;
    bus_tla.a_ready   = 1;

    code_tla.a_opcode  = sig_code_wren ? TL::PutPartialData : TL::Get;
    code_tla.a_param   = b3(DONTCARE);
    code_tla.a_size    = 2;
    code_tla.a_source  = b1(DONTCARE);
    code_tla.a_address = sig_code_addr;
    code_tla.a_mask    = sig_code_wmask;
    code_tla.a_data    = sig_code_wdata;
    code_tla.a_valid   = 1;
    code_tla.a_ready   = 1;

    core_to_reg.raddr1 = sig_rf_raddr1;
    core_to_reg.raddr2 = sig_rf_raddr2;
    core_to_reg.waddr = sig_rf_waddr;
    core_to_reg.wdata = sig_rf_wdata;
    core_to_reg.wren = sig_rf_wren;
  }

  //----------------------------------------

  void tick(logic<1> reset_in / *, logic<32> code_rdata, logic<32> bus_rdata* /) {

    if (reset_in) {
      reg_hpc_a     = 0x00400000;

      reg_hpc_b     = 0;
      reg_insn_b    = 0;

      reg_hpc_c     = 0;
      reg_insn_c    = 0;
      reg_addr_c    = 0;
      reg_result_c  = 0;

      reg_hpc_d     = 0;
      reg_insn_d    = 0;
      reg_result_d  = 0;

      reg_ticks     = 0;
    }
    else {
      reg_hpc_d     = reg_hpc_c;
      reg_insn_d    = reg_insn_c;
      reg_result_d  = sig_result_c;

      reg_hpc_c     = reg_hpc_b;
      reg_insn_c    = reg_insn_b;
      reg_addr_c    = sig_addr_b;
      reg_result_c  = sig_result_b;

      reg_hpc_b     = reg_hpc_a;
      reg_insn_b    = sig_insn_a;

      reg_hpc_a     = sig_hpc_a;

      reg_ticks     = reg_ticks + 1;
    }
  }

  //----------------------------------------

  tilelink_a bus_tla;
  tilelink_a code_tla;
*/

/*private:*/

  regfile_in core_to_reg;

  //----------------------------------------
  // Signals to code ram

  logic[31:0] sig_code_addr;
  logic[31:0] sig_code_wdata;
  logic[3:0]  sig_code_wmask;
  logic  sig_code_wren;

  //----------------------------------------
  // Signals to data bus

  logic[31:0] sig_bus_addr;
  logic  sig_bus_rden;
  logic[31:0] sig_bus_wdata;
  logic[3:0]  sig_bus_wmask;
  logic  sig_bus_wren;

  //----------------------------------------
  // Signals to regfile

  logic[7:0]  sig_rf_raddr1;
  logic[7:0]  sig_rf_raddr2;
  logic[7:0]  sig_rf_waddr;
  logic[31:0] sig_rf_wdata;
  logic  sig_rf_wren;

  //----------------------------------------
  // Internal signals and registers
  // metron_internal

  logic[31:0] sig_hpc_a;
  logic[31:0] reg_hpc_a;
  logic[31:0] sig_insn_a;

  logic[31:0] reg_hpc_b;
  logic[31:0] reg_insn_b;
  logic[31:0] sig_addr_b;
  logic[31:0] sig_result_b;

  logic[31:0] reg_hpc_c;
  logic[31:0] reg_insn_c;
  logic[31:0] reg_addr_c;
  logic[31:0] sig_result_c;
  logic[31:0] reg_result_c;

  logic[31:0] reg_hpc_d;
  logic[31:0] reg_insn_d;
  logic[31:0] reg_result_d;

  logic[31:0] reg_ticks;

  //----------------------------------------
  // FIXME support static

  function logic[31:0] decode_imm(logic[31:0] insn) /*const*/;
    logic[4:0]  op;
    logic[31:0] imm_i;
    logic[31:0] imm_s;
    logic[31:0] imm_u;
    logic[31:0] imm_b;
    logic[31:0] imm_j;
    logic[31:0] result;
    op    = insn[6:2];
    imm_i = $signed(insn[31:20]);
    imm_s = {{21 {insn[31]}}, insn[30:25], insn[11:7]};
    imm_u = insn[31:12] << 12;
    imm_b = {{20 {insn[31]}}, insn[7], insn[30:25], insn[11:8], 1'd0};
    imm_j = {{12 {insn[31]}}, insn[19:12], insn[20], insn[30:21], 1'd0};

    case(op)
      RV32I::OP_LOAD:   result = imm_i; /*break;*/
      RV32I::OP_OPIMM:  result = imm_i; /*break;*/
      RV32I::OP_AUIPC:  result = imm_u; /*break;*/
      RV32I::OP_STORE:  result = imm_s; /*break;*/
      RV32I::OP_OP:     result = imm_i; /*break;*/
      RV32I::OP_LUI:    result = imm_u; /*break;*/
      RV32I::OP_BRANCH: result = imm_b; /*break;*/
      RV32I::OP_JALR:   result = imm_i; /*break;*/
      RV32I::OP_JAL:    result = imm_j; /*break;*/
      default:              result = 0;     /*break;*/
    endcase
    decode_imm = result;
  endfunction

  //----------------------------------------

  always_comb begin : execute_alu
    logic[4:0]  op;
    logic[2:0]  f3;
    logic[6:0]  f7;
    logic[31:0] imm;
    logic[31:0] alu_a;
    logic[31:0] alu_b;
    logic[31:0] result;
    op  = execute_alu_insn[6:2];
    f3  = execute_alu_insn[14:12];
    f7  = execute_alu_insn[31:25];
    imm = decode_imm(execute_alu_insn);

    alu_a = execute_alu_reg_a;
    alu_b = op == RV32I::OP_OPIMM ? imm : execute_alu_reg_b;
    if (op == RV32I::OP_OP && f3 == 0 && f7 == 32) alu_b = -alu_b;

    case (f3)
      0:  result = alu_a + alu_b; /*break;*/
      1:  result = alu_a << 5'(alu_b); /*break;*/
      2:  result = $signed(alu_a) < $signed(alu_b); /*break;*/
      3:  result = alu_a < alu_b; /*break;*/
      4:  result = alu_a ^ alu_b; /*break;*/
      5:  result = f7 == 32 ? $signed(alu_a) >> 5'(alu_b) : alu_a >> 5'(alu_b); /*break;*/
      6:  result = alu_a | alu_b; /*break;*/
      7:  result = alu_a & alu_b; /*break;*/
      default: result = 0;
    endcase
    execute_alu_ret = result;
  end

  //----------------------------------------

  always_comb begin : execute_system
    logic[2:0]  f3;
    logic[11:0] csr;
    logic[31:0] result;
    f3  = execute_system_insn[14:12];
    csr = execute_system_insn[31:20];

    // FIXME need a good error if case is missing an expression
    result = 0;
    case(f3)
      RV32I::F3_CSRRW: begin
        result = execute_system_reg_a;
        /*break;*/
      end
      RV32I::F3_CSRRS: begin
        if (csr == 12'hF14) result = reg_hpc_b[31:24];
        /*break;*/
      end
      RV32I::F3_CSRRC:  result = 0; /*break;*/
      RV32I::F3_CSRRWI: result = 0; /*break;*/
      RV32I::F3_CSRRSI: result = 0; /*break;*/
      RV32I::F3_CSRRCI: result = 0; /*break;*/
    endcase
    execute_system_ret = result;
>>>>>>> Stashed changes
  end

endmodule

/* verilator lint_on UNUSEDSIGNAL */

`endif
