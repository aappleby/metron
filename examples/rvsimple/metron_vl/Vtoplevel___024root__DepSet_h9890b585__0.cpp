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
    IData/*31:0*/ toplevel__DOT__data_memory_bus_q;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath_alu_result_equal_zero;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_imm_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__branch_funct;
    IData/*31:0*/ toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix;
    CData/*0:0*/ toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr;
    CData/*0:0*/ toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory;
    IData/*31:0*/ __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask;
    CData/*7:0*/ __Vtableidx1;
    CData/*6:0*/ __Vtableidx2;
    CData/*6:0*/ __Vtableidx3;
    CData/*6:0*/ __Vtableidx4;
    CData/*6:0*/ __Vtableidx5;
    CData/*3:0*/ __Vtableidx6;
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0;
    SData/*14:0*/ __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    // Body
    __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 = 0U;
    __Vdlyvset__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0 = 0U;
    if ((((0x23U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
          & (0x80000000U <= vlSelf->toplevel__DOT__core_alu_result)) 
         & (0x8001ffffU >= vlSelf->toplevel__DOT__core_alu_result))) {
        __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask = 0U;
        if ((1U & (IData)(vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xffU | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        if ((2U & (IData)(vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xff00U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        if ((4U & (IData)(vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xff0000U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        if ((8U & (IData)(vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xff000000U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 
            = ((vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                [(0x7fffU & (vlSelf->toplevel__DOT__core_alu_result 
                             >> 2U))] & (~ __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask)) 
               | (vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data 
                  & __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask));
        __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 = 1U;
        __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 
            = (0x7fffU & (vlSelf->toplevel__DOT__core_alu_result 
                          >> 2U));
    }
    if (((IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable) 
         & (0U != (0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                            >> 7U))))) {
        __Vdlyvval__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0 
            = (((((((((0U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select)) 
                      | (1U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                     | (2U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                    | (3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                   | (4U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                  | (5U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                 | (6U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                | (7U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select)))
                ? ((0U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))
                    ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result
                    : ((1U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))
                        ? vlSelf->toplevel__DOT__core__DOT__dmem_read_data
                        : ((2U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))
                            ? ((IData)(4U) + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value)
                            : ((3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))
                                ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                : 0U)))) : 0U);
        __Vdlyvset__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0 = 1U;
        __Vdlyvdim0__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0 
            = (0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                        >> 7U));
    }
    if (__Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0] 
            = __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    }
    if (__Vdlyvset__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs[__Vdlyvdim0__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0] 
            = __Vdlyvval__toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs__v0;
    }
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value 
        = ((IData)(vlSelf->reset) ? 0x400000U : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out);
    vlSelf->o_pc = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value;
    toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr 
        = ((0x400000U <= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value) 
           & (0x40ffffU >= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value));
    vlSelf->toplevel__DOT__text_memory_bus_read_data 
        = ((IData)(toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr)
            ? vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem
           [(0x3fffU & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value 
                        >> 2U))] : 0U);
    vlSelf->o_inst = vlSelf->toplevel__DOT__text_memory_bus_read_data;
    vlSelf->o_bus_write_enable = (0x23U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data));
    vlSelf->o_bus_read_enable = (3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data));
    __Vtableidx2 = (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data);
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable 
        = Vtoplevel__ConstPool__TABLE_hcae45265_0[__Vtableidx2];
    __Vtableidx5 = (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data);
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select 
        = Vtoplevel__ConstPool__TABLE_h7ffbd43f_0[__Vtableidx5];
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select 
        = (((((((((0x17U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
                  | (0x6fU == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
                 | (0x33U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
                | (0x37U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
               | (0x63U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
              | (3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
             | (0x23U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
            | (0x13U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
           & ((0x17U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
              | (0x6fU == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))));
    __Vtableidx3 = (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data);
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select 
        = Vtoplevel__ConstPool__TABLE_h0f0752ae_0[__Vtableidx3];
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate 
        = (((((((((3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
                  | (7U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
                 | (0x13U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
                | (0x67U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
               | (0x27U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
              | (0x23U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
             | (0x63U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
            | (0x17U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)))
            ? ((3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                ? (((- (IData)((vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                >> 0x1fU))) << 0xbU) 
                   | (0x7ffU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                >> 0x14U))) : ((7U 
                                                == 
                                                (0x7fU 
                                                 & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                ? (
                                                   ((- (IData)(
                                                               (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                >> 0x1fU))) 
                                                    << 0xbU) 
                                                   | (0x7ffU 
                                                      & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                         >> 0x14U)))
                                                : (
                                                   (0x13U 
                                                    == 
                                                    (0x7fU 
                                                     & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                    ? 
                                                   (((- (IData)(
                                                                (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                 >> 0x1fU))) 
                                                     << 0xbU) 
                                                    | (0x7ffU 
                                                       & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                          >> 0x14U)))
                                                    : 
                                                   ((0x67U 
                                                     == 
                                                     (0x7fU 
                                                      & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                     ? 
                                                    (((- (IData)(
                                                                 (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                  >> 0x1fU))) 
                                                      << 0xbU) 
                                                     | (0x7ffU 
                                                        & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                           >> 0x14U)))
                                                     : 
                                                    ((0x27U 
                                                      == 
                                                      (0x7fU 
                                                       & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                      ? 
                                                     (((- (IData)(
                                                                  (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                   >> 0x1fU))) 
                                                       << 0xbU) 
                                                      | ((0x7e0U 
                                                          & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                             >> 0x14U)) 
                                                         | (0x1fU 
                                                            & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                               >> 7U))))
                                                      : 
                                                     ((0x23U 
                                                       == 
                                                       (0x7fU 
                                                        & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                       ? 
                                                      (((- (IData)(
                                                                   (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                    >> 0x1fU))) 
                                                        << 0xbU) 
                                                       | ((0x7e0U 
                                                           & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                              >> 0x14U)) 
                                                          | (0x1fU 
                                                             & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                >> 7U))))
                                                       : 
                                                      ((0x63U 
                                                        == 
                                                        (0x7fU 
                                                         & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                        ? 
                                                       (((- (IData)(
                                                                    (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                     >> 0x1fU))) 
                                                         << 0xcU) 
                                                        | ((0x800U 
                                                            & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                               << 4U)) 
                                                           | ((0x7e0U 
                                                               & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                  >> 0x14U)) 
                                                              | (0x1eU 
                                                                 & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                    >> 7U)))))
                                                        : 
                                                       (0xfffff000U 
                                                        & vlSelf->toplevel__DOT__text_memory_bus_read_data))))))))
            : ((0x37U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                ? (0xfffff000U & vlSelf->toplevel__DOT__text_memory_bus_read_data)
                : ((0x6fU == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                    ? (((- (IData)((vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                    >> 0x1fU))) << 0x14U) 
                       | ((0xff000U & vlSelf->toplevel__DOT__text_memory_bus_read_data) 
                          | ((0x800U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                        >> 9U)) | (
                                                   (0x7e0U 
                                                    & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                       >> 0x14U)) 
                                                   | (0x1eU 
                                                      & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                         >> 0x14U))))))
                    : 0U)));
    toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data 
        = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
        [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                   >> 0x14U))];
    __Vtableidx4 = (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data);
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2 
        = Vtoplevel__ConstPool__TABLE_hf7367149_0[__Vtableidx4];
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct 
        = (((((((((0U == (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                >> 0xcU))) | (1U == 
                                              (7U & 
                                               (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                >> 0xcU)))) 
                 | (2U == (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                 >> 0xcU)))) | (3U 
                                                == 
                                                (7U 
                                                 & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                    >> 0xcU)))) 
               | (4U == (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                               >> 0xcU)))) | (5U == 
                                              (7U & 
                                               (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                >> 0xcU)))) 
             | (6U == (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                             >> 0xcU)))) | (7U == (7U 
                                                   & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                      >> 0xcU))))
            ? ((0U == (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                             >> 0xcU))) ? 1U : ((1U 
                                                 == 
                                                 (7U 
                                                  & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                     >> 0xcU)))
                                                 ? 3U
                                                 : 
                                                ((2U 
                                                  == 
                                                  (7U 
                                                   & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                      >> 0xcU)))
                                                  ? 7U
                                                  : 
                                                 ((3U 
                                                   == 
                                                   (7U 
                                                    & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                       >> 0xcU)))
                                                   ? 8U
                                                   : 
                                                  ((4U 
                                                    == 
                                                    (7U 
                                                     & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                        >> 0xcU)))
                                                    ? 9U
                                                    : 
                                                   ((5U 
                                                     == 
                                                     (7U 
                                                      & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                         >> 0xcU)))
                                                     ? 4U
                                                     : 
                                                    ((6U 
                                                      == 
                                                      (7U 
                                                       & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                          >> 0xcU)))
                                                      ? 0xaU
                                                      : 0xbU)))))))
            : 0U);
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct 
        = ((0U == (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                         >> 0xcU))) ? 2U : ((5U == 
                                             (7U & 
                                              (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                               >> 0xcU)))
                                             ? 5U : 0U));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_funct 
        = ((0x40000000U & vlSelf->toplevel__DOT__text_memory_bus_read_data)
            ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct)
            : (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_imm_funct 
        = ((IData)((0x40001000U == (0x40003000U & vlSelf->toplevel__DOT__text_memory_bus_read_data)))
            ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct)
            : (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__branch_funct 
        = ((0U == (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                         >> 0xcU))) ? 6U : ((1U == 
                                             (7U & 
                                              (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                               >> 0xcU)))
                                             ? 6U : 
                                            ((4U == 
                                              (7U & 
                                               (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                >> 0xcU)))
                                              ? 7U : 
                                             ((5U == 
                                               (7U 
                                                & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                   >> 0xcU)))
                                               ? 7U
                                               : ((6U 
                                                   == 
                                                   (7U 
                                                    & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                       >> 0xcU)))
                                                   ? 8U
                                                   : 
                                                  ((7U 
                                                    == 
                                                    (7U 
                                                     & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                        >> 0xcU)))
                                                    ? 8U
                                                    : 0U))))));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
            ? 1U : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                     ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_funct)
                     : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                         ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_imm_funct)
                         : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                             ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__branch_funct)
                             : 0U))));
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
        = (((((((((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function)) 
                  | (2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
                 | (3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
                | (4U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
               | (5U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
              | (6U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
             | (7U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
            | (8U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function)))
            ? ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                     ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                         : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                    [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                               >> 0xfU))]) + ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                               ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                   ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                   : 0U)
                                               : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                    ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                             : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                        [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                   >> 0xfU))]) - ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                   ? 
                                                  ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                    ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                    : 0U)
                                                   : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                    : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                        ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                 ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                                 : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                            [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                       >> 0xfU))]) 
                           << (0x1fU & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                             ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                             : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))
                        : ((4U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                            ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                 ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                     ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                                     : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                           >> 0xfU))]) 
                               >> (0x1fU & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                 ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                 : 0U)
                                             : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))
                            : ((5U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                                ? VL_SHIFTRS_III(32,32,5, 
                                                 ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                   ? 
                                                  ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                    ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                                                    : 0U)
                                                   : 
                                                  vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                                  [
                                                  (0x1fU 
                                                   & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                      >> 0xfU))]), 
                                                 (0x1fU 
                                                  & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                      ? 
                                                     ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                       ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                       : 0U)
                                                      : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))
                                : ((6U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                                    ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                             ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                                             : 0U) : 
                                        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                        [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                   >> 0xfU))]) 
                                       == ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                            ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                : 0U)
                                            : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                                    : ((7U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                                        ? VL_LTS_III(32, 
                                                     ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                       ? 
                                                      ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                        ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                                                        : 0U)
                                                       : 
                                                      vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                                      [
                                                      (0x1fU 
                                                       & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                          >> 0xfU))]), 
                                                     ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                       ? 
                                                      ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                        ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                        : 0U)
                                                       : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                                        : (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                                 ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                                                 : 0U)
                                             : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                                            [(0x1fU 
                                              & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                 >> 0xfU))]) 
                                           < ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                               ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                   ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                   : 0U)
                                               : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data)))))))))
            : ((9U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                     ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                         : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                    [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                               >> 0xfU))]) ^ ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                               ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                   ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                   : 0U)
                                               : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                : ((0xaU == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                    ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                         ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                             : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                        [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                   >> 0xfU))]) | ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                   ? 
                                                  ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                                    ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                                    : 0U)
                                                   : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                    : ((0xbU == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                        ? (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                             ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                                 ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value
                                 : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
                            [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                       >> 0xfU))]) 
                           & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                               ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                   ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                   : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                        : 0U))));
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data 
        = ((0x1fU >= (0x18U & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
                               << 3U))) ? (toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data 
                                           << (0x18U 
                                               & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
                                                  << 3U)))
            : 0U);
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable 
        = (0xfU & ((0U == (3U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                 >> 0xcU))) ? ((IData)(1U) 
                                               << (3U 
                                                   & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result))
                    : ((1U == (3U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                     >> 0xcU))) ? ((IData)(3U) 
                                                   << 
                                                   (3U 
                                                    & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result))
                        : ((2U == (3U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                         >> 0xcU)))
                            ? ((IData)(0xfU) << (3U 
                                                 & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result))
                            : 0U))));
    vlSelf->toplevel__DOT__core_alu_result = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result;
    toplevel__DOT__core__DOT__ctlpath_alu_result_equal_zero 
        = (0U == vlSelf->toplevel__DOT__core_alu_result);
    vlSelf->o_bus_write_data = vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data;
    vlSelf->o_bus_byte_enable = vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable;
    vlSelf->o_bus_address = vlSelf->toplevel__DOT__core_alu_result;
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory 
        = ((0x80000000U <= vlSelf->toplevel__DOT__core_alu_result) 
           & (0x8001ffffU >= vlSelf->toplevel__DOT__core_alu_result));
    toplevel__DOT__data_memory_bus_q = (((3U == (0x7fU 
                                                 & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
                                         & (IData)(toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory))
                                         ? vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                                        [(0x7fffU & 
                                          (vlSelf->toplevel__DOT__core_alu_result 
                                           >> 2U))]
                                         : 0U);
    __Vtableidx6 = (((IData)(toplevel__DOT__core__DOT__ctlpath_alu_result_equal_zero) 
                     << 3U) | (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                     >> 0xcU)));
    toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch 
        = Vtoplevel__ConstPool__TABLE_hfd21f17e_0[__Vtableidx6];
    vlSelf->o_bus_read_data = toplevel__DOT__data_memory_bus_q;
    toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix 
        = ((0x1fU >= (0x18U & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
                               << 3U))) ? (toplevel__DOT__data_memory_bus_q 
                                           >> (0x18U 
                                               & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
                                                  << 3U)))
            : 0U);
    vlSelf->toplevel__DOT__core__DOT__dmem_read_data 
        = ((0U == (3U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                         >> 0xcU))) ? (((- (IData)(
                                                   (1U 
                                                    & ((~ 
                                                        (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                         >> 0xeU)) 
                                                       & (toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix 
                                                          >> 7U))))) 
                                        << 8U) | (0xffU 
                                                  & toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix))
            : ((1U == (3U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                             >> 0xcU))) ? (((- (IData)(
                                                       (1U 
                                                        & ((~ 
                                                            (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                             >> 0xeU)) 
                                                           & (toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix 
                                                              >> 0xfU))))) 
                                            << 0x10U) 
                                           | (0xffffU 
                                              & toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix))
                : ((2U == (3U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                 >> 0xcU))) ? toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__position_fix
                    : 0U)));
    __Vtableidx1 = (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch) 
                     << 7U) | (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data));
    toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select 
        = Vtoplevel__ConstPool__TABLE_h595af980_0[__Vtableidx1];
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select))
            ? ((IData)(4U) + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value)
            : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select))
                ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value 
                   + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate)
                : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select))
                    ? (0xfffffffeU & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result)
                    : 0U)));
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
