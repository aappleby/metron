// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel___024root.h"

extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_hcae45265_0;
extern const VlUnpacked<CData/*2:0*/, 128> Vtoplevel__ConstPool__TABLE_h7ffbd43f_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_h0f0752ae_0;
extern const VlUnpacked<CData/*1:0*/, 128> Vtoplevel__ConstPool__TABLE_hf7367149_0;
extern const VlUnpacked<CData/*0:0*/, 16> Vtoplevel__ConstPool__TABLE_hfd21f17e_0;
extern const VlUnpacked<CData/*1:0*/, 256> Vtoplevel__ConstPool__TABLE_h595af980_0;

VL_INLINE_OPT void Vtoplevel___024root___sequent__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___sequent__TOP__0\n"); );
    // Init
    IData/*31:0*/ toplevel__DOT__core_inst;
    IData/*31:0*/ toplevel__DOT__core_bus_read_data;
    IData/*31:0*/ toplevel__DOT__core_alu_result2;
    IData/*31:0*/ toplevel__DOT__text_memory_bus_address;
    IData/*31:0*/ toplevel__DOT__text_memory_bus_read_data;
    CData/*0:0*/ toplevel__DOT__data_memory_bus_read_enable;
    IData/*31:0*/ toplevel__DOT__data_memory_bus_read_data;
    IData/*31:0*/ toplevel__DOT__tock__DOT__pc;
    IData/*31:0*/ toplevel__DOT__tock__DOT__inst;
    IData/*31:0*/ toplevel__DOT__tock__DOT__alu_result2;
    IData/*31:0*/ toplevel__DOT__tock__DOT__write_data;
    CData/*0:0*/ toplevel__DOT__tock__DOT__write_enable;
    CData/*3:0*/ toplevel__DOT__tock__DOT__byte_enable;
    CData/*0:0*/ toplevel__DOT__tock__DOT__read_enable;
    IData/*31:0*/ toplevel__DOT__tock__DOT__read_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath_inst;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath_data_mem_read_data;
    CData/*2:0*/ toplevel__DOT__core__DOT__datapath_reg_writeback_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__datapath_next_pc_select;
    CData/*4:0*/ toplevel__DOT__core__DOT__datapath_alu_function;
    CData/*6:0*/ toplevel__DOT__core__DOT__ctlpath_inst_opcode;
    CData/*2:0*/ toplevel__DOT__core__DOT__ctlpath_inst_funct3;
    CData/*6:0*/ toplevel__DOT__core__DOT__ctlpath_inst_funct7;
    IData/*31:0*/ toplevel__DOT__core__DOT__dmem_address;
    CData/*2:0*/ toplevel__DOT__core__DOT__dmem_data_format;
    IData/*31:0*/ toplevel__DOT__core__DOT__dmem_read_data;
    CData/*6:0*/ toplevel__DOT__core__DOT__unnamedblk4__DOT__funct7;
    CData/*4:0*/ toplevel__DOT__core__DOT__unnamedblk4__DOT__alu_function;
    CData/*6:0*/ toplevel__DOT__core__DOT__tock__DOT__opcode;
    CData/*2:0*/ toplevel__DOT__core__DOT__tock__DOT__funct3;
    CData/*0:0*/ toplevel__DOT__core__DOT__tock__DOT__reg_we;
    IData/*31:0*/ toplevel__DOT__core__DOT__tock__DOT__mem_data;
    CData/*2:0*/ toplevel__DOT__core__DOT__tock__DOT__reg_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__tock__DOT__pc_select;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__alu_core_result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_in2;
    CData/*4:0*/ toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address;
    CData/*4:0*/ toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_address;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__igen_inst;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__igen_immediate;
    CData/*4:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk2__DOT__rs1_idx;
    CData/*4:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk2__DOT__rs2_idx;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_plus_4;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_plus_imm;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__reg_data;
    CData/*6:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2;
    CData/*2:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__unnamedblk1__DOT__take_branch;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__default_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__secondary_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__op_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__op_imm_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__branch_funct;
    IData/*31:0*/ toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix;
    IData/*31:0*/ toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__fetched;
    CData/*0:0*/ toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr;
    IData/*31:0*/ toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched;
    CData/*0:0*/ toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory;
    IData/*31:0*/ __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask;
    CData/*7:0*/ __Vtableidx1;
    CData/*6:0*/ __Vtableidx2;
    CData/*6:0*/ __Vtableidx3;
    CData/*6:0*/ __Vtableidx4;
    CData/*6:0*/ __Vtableidx5;
    CData/*3:0*/ __Vtableidx6;
    // Body
    if ((((IData)(vlSelf->toplevel__DOT__data_memory_bus_write_enable) 
          & (0x80000000U <= vlSelf->toplevel__DOT__data_memory_bus_address)) 
         & (0x8001ffffU >= vlSelf->toplevel__DOT__data_memory_bus_address))) {
        __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask = 0U;
        if ((1U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask 
                = (0xffU | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask);
        }
        if ((2U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask 
                = (0xff00U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask);
        }
        if ((4U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask 
                = (0xff0000U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask);
        }
        if ((8U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask 
                = (0xff000000U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask);
        }
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address] 
            = ((vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                [vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address] 
                & (~ __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask)) 
               | (vlSelf->toplevel__DOT__data_memory_bus_write_data 
                  & __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk1__DOT__mask));
    }
    if (((IData)(vlSelf->toplevel__DOT__core__DOT__datapath_regfile_write_enable) 
         & (0U != (0x1fU & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
                            >> 7U))))) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs[(0x1fU 
                                                                          & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
                                                                             >> 7U))] 
            = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs_rd_data;
    }
    if (vlSelf->reset) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value = 0x400000U;
    } else if (vlSelf->toplevel__DOT__core__DOT__datapath_pc_write_enable) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value 
            = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_next;
    }
    toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_plus_4 
        = ((IData)(4U) + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value);
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0 
        = toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_plus_4;
    toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_in2 
        = toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_plus_4;
    toplevel__DOT__tock__DOT__pc = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value;
    toplevel__DOT__text_memory_bus_address = toplevel__DOT__tock__DOT__pc;
    vlSelf->o_pc = toplevel__DOT__tock__DOT__pc;
    toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__fetched 
        = vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem
        [(0x3fffU & (toplevel__DOT__text_memory_bus_address 
                     >> 2U))];
    toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr 
        = ((0x400000U <= toplevel__DOT__text_memory_bus_address) 
           & (0x40ffffU >= toplevel__DOT__text_memory_bus_address));
    toplevel__DOT__text_memory_bus_read_data = ((IData)(toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr)
                                                 ? toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__fetched
                                                 : 0U);
    toplevel__DOT__tock__DOT__inst = toplevel__DOT__text_memory_bus_read_data;
    vlSelf->o_inst = toplevel__DOT__tock__DOT__inst;
    toplevel__DOT__core_inst = toplevel__DOT__tock__DOT__inst;
    toplevel__DOT__core__DOT__datapath_inst = toplevel__DOT__core_inst;
    toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
        = toplevel__DOT__core__DOT__datapath_inst;
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
        = toplevel__DOT__core__DOT__datapath_inst;
    toplevel__DOT__core__DOT__datapath__DOT__igen_immediate 
        = (((((((((3U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst)) 
                  | (7U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))) 
                 | (0x13U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))) 
                | (0x67U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))) 
               | (0x27U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))) 
              | (0x23U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))) 
             | (0x63U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))) 
            | (0x17U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst)))
            ? ((3U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                ? (((- (IData)((toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                >> 0x1fU))) << 0xbU) 
                   | (0x7ffU & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                >> 0x14U))) : ((7U 
                                                == 
                                                (0x7fU 
                                                 & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                                                ? (
                                                   ((- (IData)(
                                                               (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                >> 0x1fU))) 
                                                    << 0xbU) 
                                                   | (0x7ffU 
                                                      & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                         >> 0x14U)))
                                                : (
                                                   (0x13U 
                                                    == 
                                                    (0x7fU 
                                                     & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                                                    ? 
                                                   (((- (IData)(
                                                                (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                 >> 0x1fU))) 
                                                     << 0xbU) 
                                                    | (0x7ffU 
                                                       & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                          >> 0x14U)))
                                                    : 
                                                   ((0x67U 
                                                     == 
                                                     (0x7fU 
                                                      & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                                                     ? 
                                                    (((- (IData)(
                                                                 (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                  >> 0x1fU))) 
                                                      << 0xbU) 
                                                     | (0x7ffU 
                                                        & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                           >> 0x14U)))
                                                     : 
                                                    ((0x27U 
                                                      == 
                                                      (0x7fU 
                                                       & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                                                      ? 
                                                     (((- (IData)(
                                                                  (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                   >> 0x1fU))) 
                                                       << 0xbU) 
                                                      | ((0x7e0U 
                                                          & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                             >> 0x14U)) 
                                                         | (0x1fU 
                                                            & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                               >> 7U))))
                                                      : 
                                                     ((0x23U 
                                                       == 
                                                       (0x7fU 
                                                        & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                                                       ? 
                                                      (((- (IData)(
                                                                   (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                    >> 0x1fU))) 
                                                        << 0xbU) 
                                                       | ((0x7e0U 
                                                           & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                              >> 0x14U)) 
                                                          | (0x1fU 
                                                             & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                >> 7U))))
                                                       : 
                                                      ((0x63U 
                                                        == 
                                                        (0x7fU 
                                                         & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                                                        ? 
                                                       (((- (IData)(
                                                                    (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                     >> 0x1fU))) 
                                                         << 0xcU) 
                                                        | ((0x800U 
                                                            & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                               << 4U)) 
                                                           | ((0x7e0U 
                                                               & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                  >> 0x14U)) 
                                                              | (0x1eU 
                                                                 & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                                    >> 7U)))))
                                                        : 
                                                       (0xfffff000U 
                                                        & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))))))))
            : ((0x37U == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                ? (0xfffff000U & toplevel__DOT__core__DOT__datapath__DOT__igen_inst)
                : ((0x6fU == (0x7fU & toplevel__DOT__core__DOT__datapath__DOT__igen_inst))
                    ? (((- (IData)((toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                    >> 0x1fU))) << 0x14U) 
                       | ((0xff000U & toplevel__DOT__core__DOT__datapath__DOT__igen_inst) 
                          | ((0x800U & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                        >> 9U)) | (
                                                   (0x7e0U 
                                                    & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                       >> 0x14U)) 
                                                   | (0x1eU 
                                                      & (toplevel__DOT__core__DOT__datapath__DOT__igen_inst 
                                                         >> 0x14U))))))
                    : 0U)));
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk2__DOT__rs1_idx 
        = (0x1fU & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
                    >> 0xfU));
    toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk2__DOT__rs1_idx;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk2__DOT__rs2_idx 
        = (0x1fU & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
                    >> 0x14U));
    toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_address 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk2__DOT__rs2_idx;
    toplevel__DOT__core__DOT__unnamedblk4__DOT__funct7 
        = (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
           >> 0x19U);
    toplevel__DOT__core__DOT__ctlpath_inst_funct7 = toplevel__DOT__core__DOT__unnamedblk4__DOT__funct7;
    toplevel__DOT__core__DOT__tock__DOT__funct3 = (7U 
                                                   & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
                                                      >> 0xcU));
    toplevel__DOT__core__DOT__dmem_data_format = toplevel__DOT__core__DOT__tock__DOT__funct3;
    toplevel__DOT__core__DOT__ctlpath_inst_funct3 = toplevel__DOT__core__DOT__tock__DOT__funct3;
    toplevel__DOT__core__DOT__tock__DOT__opcode = (0x7fU 
                                                   & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__idec_inst);
    toplevel__DOT__core__DOT__ctlpath_inst_opcode = toplevel__DOT__core__DOT__tock__DOT__opcode;
    toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_plus_imm 
        = (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value 
           + toplevel__DOT__core__DOT__datapath__DOT__igen_immediate);
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1 
        = toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_plus_imm;
    toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data 
        = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
        [toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_address];
    toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode 
        = toplevel__DOT__core__DOT__ctlpath_inst_opcode;
    toplevel__DOT__tock__DOT__write_enable = (0x23U 
                                              == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode));
    vlSelf->o_bus_write_enable = toplevel__DOT__tock__DOT__write_enable;
    vlSelf->toplevel__DOT__data_memory_bus_write_enable 
        = toplevel__DOT__tock__DOT__write_enable;
    __Vtableidx2 = toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable 
        = Vtoplevel__ConstPool__TABLE_hcae45265_0[__Vtableidx2];
    __Vtableidx5 = toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select 
        = Vtoplevel__ConstPool__TABLE_h7ffbd43f_0[__Vtableidx5];
    toplevel__DOT__tock__DOT__read_enable = (3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode));
    toplevel__DOT__data_memory_bus_read_enable = toplevel__DOT__tock__DOT__read_enable;
    vlSelf->o_bus_read_enable = toplevel__DOT__tock__DOT__read_enable;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select 
        = (((((((((0x17U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode)) 
                  | (0x6fU == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))) 
                 | (0x33U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))) 
                | (0x37U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))) 
               | (0x63U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))) 
              | (3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))) 
             | (0x23U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))) 
            | (0x13U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))) 
           & ((0x17U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode)) 
              | (0x6fU == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))));
    __Vtableidx3 = toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select 
        = Vtoplevel__ConstPool__TABLE_h0f0752ae_0[__Vtableidx3];
    __Vtableidx4 = toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2 
        = Vtoplevel__ConstPool__TABLE_hf7367149_0[__Vtableidx4];
    toplevel__DOT__core__DOT__tock__DOT__reg_we = toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable;
    vlSelf->toplevel__DOT__core__DOT__datapath_regfile_write_enable 
        = toplevel__DOT__core__DOT__tock__DOT__reg_we;
    toplevel__DOT__core__DOT__tock__DOT__reg_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select;
    toplevel__DOT__core__DOT__datapath_reg_writeback_select 
        = toplevel__DOT__core__DOT__tock__DOT__reg_select;
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__default_funct 
        = (((((((((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3)) 
                  | (1U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
                 | (2U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
                | (3U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
               | (4U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
              | (5U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
             | (6U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
            | (7U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3)))
            ? ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                ? 1U : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                         ? 3U : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                  ? 7U : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                           ? 8U : (
                                                   (4U 
                                                    == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                    ? 9U
                                                    : 
                                                   ((5U 
                                                     == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                     ? 4U
                                                     : 
                                                    ((6U 
                                                      == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                      ? 0xaU
                                                      : 0xbU)))))))
            : 0U);
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__secondary_funct 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
            ? 2U : ((5U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                     ? 5U : 0U));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__op_funct 
        = ((0x20U & (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct7))
            ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__secondary_funct)
            : (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__default_funct));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__op_imm_funct 
        = ((((IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct7) 
             >> 5U) & (1U == (3U & (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))))
            ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__secondary_funct)
            : (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__default_funct));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__branch_funct 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
            ? 6U : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                     ? 6U : ((4U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                              ? 7U : ((5U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                       ? 7U : ((6U 
                                                == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                ? 8U
                                                : (
                                                   (7U 
                                                    == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                    ? 8U
                                                    : 0U))))));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
            ? 1U : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                     ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__op_funct)
                     : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                         ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__op_imm_funct)
                         : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                             ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__unnamedblk1__DOT__branch_funct)
                             : 0U))));
    toplevel__DOT__core__DOT__unnamedblk4__DOT__alu_function 
        = toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function;
    toplevel__DOT__core__DOT__datapath_alu_function 
        = toplevel__DOT__core__DOT__unnamedblk4__DOT__alu_function;
    toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
        = (((((((((1U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function)) 
                  | (2U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))) 
                 | (3U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))) 
                | (4U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))) 
               | (5U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))) 
              | (6U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))) 
             | (7U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))) 
            | (8U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function)))
            ? ((1U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                     ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                         : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                    [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                   + ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                       ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                           ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                           : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                : ((2U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                    ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                             : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                        [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                       - ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                           ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                               ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                               : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                    : ((3U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                        ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                 ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                                 : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                            [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                           << (0x1fU & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                             ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                             : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))
                        : ((4U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                            ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                 ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                     ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                                     : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                               >> (0x1fU & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                 ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                 : 0U)
                                             : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))
                            : ((5U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                                ? VL_SHIFTRS_III(32,32,5, 
                                                 ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                   ? 
                                                  ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                    ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                                                    : 0U)
                                                   : 
                                                  vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                                  [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]), 
                                                 (0x1fU 
                                                  & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                      ? 
                                                     ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                       ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                       : 0U)
                                                      : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))
                                : ((6U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                                    ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                             ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                                             : 0U) : 
                                        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                        [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                                       == ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                            ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                : 0U)
                                            : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                                    : ((7U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                                        ? VL_LTS_III(32, 
                                                     ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                       ? 
                                                      ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                        ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                                                        : 0U)
                                                       : 
                                                      vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                                      [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]), 
                                                     ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                       ? 
                                                      ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                        ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                        : 0U)
                                                       : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                                        : (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                 ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                                                 : 0U)
                                             : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                            [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                                           < ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                               ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                   ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                   : 0U)
                                               : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))))))))
            : ((9U == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                     ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                         : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                    [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                   ^ ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                       ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                           ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                           : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                : ((0xaU == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                    ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                             : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                        [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                       | ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                           ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                               ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                               : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                    : ((0xbU == (IData)(toplevel__DOT__core__DOT__datapath_alu_function))
                        ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                 ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                                 : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                            [toplevel__DOT__core__DOT__datapath__DOT__regs_rs1_address]) 
                           & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                               ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                   ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                   : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                        : 0U))));
    toplevel__DOT__tock__DOT__alu_result2 = toplevel__DOT__core__DOT__datapath__DOT__alu_core_result;
    vlSelf->o_bus_address = toplevel__DOT__tock__DOT__alu_result2;
    vlSelf->toplevel__DOT__data_memory_bus_address 
        = toplevel__DOT__tock__DOT__alu_result2;
    toplevel__DOT__core_alu_result2 = toplevel__DOT__tock__DOT__alu_result2;
    toplevel__DOT__core__DOT__dmem_address = toplevel__DOT__core_alu_result2;
    __Vtableidx6 = (((0U == toplevel__DOT__core_alu_result2) 
                     << 3U) | (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3));
    toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch 
        = Vtoplevel__ConstPool__TABLE_hfd21f17e_0[__Vtableidx6];
    vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address 
        = (0x7fffU & (vlSelf->toplevel__DOT__data_memory_bus_address 
                      >> 2U));
    toplevel__DOT__tock__DOT__write_data = ((0x1fU 
                                             >= (0x18U 
                                                 & (toplevel__DOT__core__DOT__dmem_address 
                                                    << 3U)))
                                             ? (toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data 
                                                << 
                                                (0x18U 
                                                 & (toplevel__DOT__core__DOT__dmem_address 
                                                    << 3U)))
                                             : 0U);
    vlSelf->o_bus_write_data = toplevel__DOT__tock__DOT__write_data;
    vlSelf->toplevel__DOT__data_memory_bus_write_data 
        = toplevel__DOT__tock__DOT__write_data;
    toplevel__DOT__tock__DOT__byte_enable = (0xfU & 
                                             ((0U == 
                                               (3U 
                                                & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                               ? ((IData)(1U) 
                                                  << 
                                                  (3U 
                                                   & toplevel__DOT__core__DOT__dmem_address))
                                               : ((1U 
                                                   == 
                                                   (3U 
                                                    & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                   ? 
                                                  ((IData)(3U) 
                                                   << 
                                                   (3U 
                                                    & toplevel__DOT__core__DOT__dmem_address))
                                                   : 
                                                  ((2U 
                                                    == 
                                                    (3U 
                                                     & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                    ? 
                                                   ((IData)(0xfU) 
                                                    << 
                                                    (3U 
                                                     & toplevel__DOT__core__DOT__dmem_address))
                                                    : 0U))));
    vlSelf->o_bus_byte_enable = toplevel__DOT__tock__DOT__byte_enable;
    vlSelf->toplevel__DOT__data_memory_bus_byte_enable 
        = toplevel__DOT__tock__DOT__byte_enable;
    toplevel__DOT__core__DOT__ctlpath__DOT__unnamedblk1__DOT__take_branch 
        = toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch 
        = toplevel__DOT__core__DOT__ctlpath__DOT__unnamedblk1__DOT__take_branch;
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched 
        = vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
        [vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address];
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory 
        = ((0x80000000U <= vlSelf->toplevel__DOT__data_memory_bus_address) 
           & (0x8001ffffU >= vlSelf->toplevel__DOT__data_memory_bus_address));
    toplevel__DOT__data_memory_bus_read_data = (((IData)(toplevel__DOT__data_memory_bus_read_enable) 
                                                 & (IData)(toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory))
                                                 ? toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched
                                                 : 0U);
    __Vtableidx1 = (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch) 
                     << 7U) | (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode));
    toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select 
        = Vtoplevel__ConstPool__TABLE_h595af980_0[__Vtableidx1];
    toplevel__DOT__tock__DOT__read_data = toplevel__DOT__data_memory_bus_read_data;
    vlSelf->o_bus_read_data = toplevel__DOT__tock__DOT__read_data;
    toplevel__DOT__core_bus_read_data = toplevel__DOT__tock__DOT__read_data;
    toplevel__DOT__core__DOT__tock__DOT__pc_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
    toplevel__DOT__core__DOT__datapath_next_pc_select 
        = toplevel__DOT__core__DOT__tock__DOT__pc_select;
    toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix 
        = ((0x1fU >= (0x18U & (toplevel__DOT__core__DOT__dmem_address 
                               << 3U))) ? (toplevel__DOT__core_bus_read_data 
                                           >> (0x18U 
                                               & (toplevel__DOT__core__DOT__dmem_address 
                                                  << 3U)))
            : 0U);
    toplevel__DOT__core__DOT__dmem_read_data = ((0U 
                                                 == 
                                                 (3U 
                                                  & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                 ? 
                                                (((- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  ((IData)(toplevel__DOT__core__DOT__dmem_data_format) 
                                                                   >> 2U)) 
                                                                 & (toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix 
                                                                    >> 7U))))) 
                                                  << 8U) 
                                                 | (0xffU 
                                                    & toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix))
                                                 : 
                                                ((1U 
                                                  == 
                                                  (3U 
                                                   & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                  ? 
                                                 (((- (IData)(
                                                              (1U 
                                                               & ((~ 
                                                                   ((IData)(toplevel__DOT__core__DOT__dmem_data_format) 
                                                                    >> 2U)) 
                                                                  & (toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix 
                                                                     >> 0xfU))))) 
                                                   << 0x10U) 
                                                  | (0xffffU 
                                                     & toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix))
                                                  : 
                                                 ((2U 
                                                   == 
                                                   (3U 
                                                    & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                   ? toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix
                                                   : 0U)));
    toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_data 
        = ((0U == (IData)(toplevel__DOT__core__DOT__datapath_next_pc_select))
            ? toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0
            : ((1U == (IData)(toplevel__DOT__core__DOT__datapath_next_pc_select))
                ? toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1
                : ((2U == (IData)(toplevel__DOT__core__DOT__datapath_next_pc_select))
                    ? (0xfffffffeU & toplevel__DOT__core_alu_result2)
                    : 0U)));
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_next 
        = toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__pc_data;
    toplevel__DOT__core__DOT__tock__DOT__mem_data = toplevel__DOT__core__DOT__dmem_read_data;
    toplevel__DOT__core__DOT__datapath_data_mem_read_data 
        = toplevel__DOT__core__DOT__tock__DOT__mem_data;
    toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__reg_data 
        = (((((((((0U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select)) 
                  | (1U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))) 
                 | (2U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))) 
                | (3U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))) 
               | (4U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))) 
              | (5U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))) 
             | (6U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))) 
            | (7U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select)))
            ? ((0U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))
                ? toplevel__DOT__core_alu_result2 : 
               ((1U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))
                 ? toplevel__DOT__core__DOT__datapath_data_mem_read_data
                 : ((2U == (IData)(toplevel__DOT__core__DOT__datapath_reg_writeback_select))
                     ? toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_in2
                     : toplevel__DOT__core__DOT__datapath__DOT__igen_immediate)))
            : 0U);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs_rd_data 
        = toplevel__DOT__core__DOT__datapath__DOT__tock__DOT__reg_data;
}

void Vtoplevel___024root___eval(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval\n"); );
    // Body
    if (((IData)(vlSelf->clock) & (~ (IData)(vlSelf->__Vclklast__TOP__clock)))) {
        Vtoplevel___024root___sequent__TOP__0(vlSelf);
    }
    // Final
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
}

#ifdef VL_DEBUG
void Vtoplevel___024root___eval_debug_assertions(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clock & 0xfeU))) {
        Verilated::overWidthError("clock");}
    if (VL_UNLIKELY((vlSelf->reset & 0xfeU))) {
        Verilated::overWidthError("reset");}
}
#endif  // VL_DEBUG
