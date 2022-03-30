// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel___024root.h"

VL_INLINE_OPT void Vtoplevel___024root___sequent__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___sequent__TOP__0\n"); );
    // Init
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0;
    SData/*14:0*/ __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    CData/*4:0*/ __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    CData/*7:0*/ __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0;
    SData/*14:0*/ __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1;
    CData/*4:0*/ __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1;
    CData/*7:0*/ __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1;
    SData/*14:0*/ __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2;
    CData/*4:0*/ __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2;
    CData/*7:0*/ __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2;
    SData/*14:0*/ __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3;
    CData/*4:0*/ __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3;
    CData/*7:0*/ __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3;
    // Body
    __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 = 0U;
    __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1 = 0U;
    __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2 = 0U;
    __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3 = 0U;
    __Vdlyvset__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0 = 0U;
    if (((IData)(vlSelf->bus_write_enable) & (IData)(vlSelf->toplevel__DOT__data_memory_bus__DOT__is_data_memory))) {
        if ((1U & (IData)(vlSelf->bus_byte_enable))) {
            __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 
                = (0xffU & vlSelf->bus_write_data);
            __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 = 1U;
            __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 = 0U;
            __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0 
                = (0x7fffU & (vlSelf->bus_address >> 2U));
        }
        if ((2U & (IData)(vlSelf->bus_byte_enable))) {
            __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1 
                = (0xffU & (vlSelf->bus_write_data 
                            >> 8U));
            __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1 = 1U;
            __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1 = 8U;
            __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1 
                = (0x7fffU & (vlSelf->bus_address >> 2U));
        }
        if ((4U & (IData)(vlSelf->bus_byte_enable))) {
            __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2 
                = (0xffU & (vlSelf->bus_write_data 
                            >> 0x10U));
            __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2 = 1U;
            __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2 = 0x10U;
            __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2 
                = (0x7fffU & (vlSelf->bus_address >> 2U));
        }
        if ((8U & (IData)(vlSelf->bus_byte_enable))) {
            __Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3 
                = (vlSelf->bus_write_data >> 0x18U);
            __Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3 = 1U;
            __Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3 = 0x18U;
            __Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3 
                = (0x7fffU & (vlSelf->bus_address >> 2U));
        }
    }
    if (vlSelf->toplevel__DOT__riscv_core__DOT__regfile_write_enable) {
        if ((0U != (0x1fU & (vlSelf->inst >> 7U)))) {
            __Vdlyvval__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0 
                = vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array
                [vlSelf->toplevel__DOT__riscv_core__DOT__reg_writeback_select];
            __Vdlyvset__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0 = 1U;
            __Vdlyvdim0__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0 
                = (0x1fU & (vlSelf->inst >> 7U));
        }
    }
    if (__Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0))) 
                & vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                [__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0) 
                                   << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v0))));
    }
    if (__Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1))) 
                & vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                [__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1) 
                                   << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v1))));
    }
    if (__Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2))) 
                & vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                [__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2) 
                                   << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v2))));
    }
    if (__Vdlyvset__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3] 
            = (((~ ((IData)(0xffU) << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3))) 
                & vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                [__Vdlyvdim0__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3]) 
               | (0xffffffffULL & ((IData)(__Vdlyvval__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3) 
                                   << (IData)(__Vdlyvlsb__toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem__v3))));
    }
    if (__Vdlyvset__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0) {
        vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register[__Vdlyvdim0__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0] 
            = __Vdlyvval__toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register__v0;
    }
}

extern const VlUnpacked<CData/*4:0*/, 8> Vtoplevel__ConstPool__TABLE_hacc6fcf3_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_h888b3f2b_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_hcae45265_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_he0c5286b_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_h0f0752ae_0;
extern const VlUnpacked<CData/*1:0*/, 128> Vtoplevel__ConstPool__TABLE_hf7367149_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_h5c001984_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_h5b40c0e4_0;
extern const VlUnpacked<CData/*2:0*/, 128> Vtoplevel__ConstPool__TABLE_h7ffbd43f_0;
extern const VlUnpacked<CData/*0:0*/, 16> Vtoplevel__ConstPool__TABLE_hfd21f17e_0;

VL_INLINE_OPT void Vtoplevel___024root___sequent__TOP__1(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___sequent__TOP__1\n"); );
    // Init
    CData/*0:0*/ toplevel__DOT__riscv_core__DOT__alu_operand_a_select;
    CData/*0:0*/ toplevel__DOT__riscv_core__DOT__alu_operand_b_select;
    CData/*4:0*/ toplevel__DOT__riscv_core__DOT__alu_function;
    CData/*0:0*/ toplevel__DOT__riscv_core__DOT__read_enable;
    CData/*0:0*/ toplevel__DOT__riscv_core__DOT__write_enable;
    IData/*31:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__rs2_data;
    IData/*31:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a;
    IData/*31:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b;
    IData/*31:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result;
    IData/*31:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__immediate;
    VlWide<4>/*127:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus;
    QData/*63:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT____Vcellinp__multiplexer__in_bus;
    QData/*63:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT____Vcellinp__multiplexer__in_bus;
    VlWide<8>/*255:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus;
    CData/*1:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_op_type;
    CData/*4:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__default_funct;
    CData/*4:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__secondary_funct;
    IData/*31:0*/ toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix;
    CData/*6:0*/ __Vtableidx1;
    CData/*3:0*/ __Vtableidx2;
    CData/*2:0*/ __Vtableidx3;
    VlWide<3>/*95:0*/ __Vtemp_hccccb3ad__0;
    // Body
    if (vlSelf->reset) {
        vlSelf->pc = 0x400000U;
    } else if (vlSelf->toplevel__DOT__riscv_core__DOT__pc_write_enable) {
        vlSelf->pc = vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT__multiplexer__DOT__input_array
            [((0x63U == (0x7fU & vlSelf->inst)) ? ((IData)(vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__take_branch)
                                                    ? 1U
                                                    : 0U)
               : ((0x67U == (0x7fU & vlSelf->inst))
                   ? 2U : ((0x6fU == (0x7fU & vlSelf->inst))
                            ? 1U : 0U)))];
    }
    vlSelf->inst = (((0x400000U <= vlSelf->pc) & (0x40ffffU 
                                                  >= vlSelf->pc))
                     ? vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem
                    [(0x3fffU & (vlSelf->pc >> 2U))]
                     : 0U);
    toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__secondary_funct 
        = ((0U == (7U & (vlSelf->inst >> 0xcU))) ? 2U
            : ((5U == (7U & (vlSelf->inst >> 0xcU)))
                ? 5U : 0U));
    __Vtableidx3 = (7U & (vlSelf->inst >> 0xcU));
    toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__default_funct 
        = Vtoplevel__ConstPool__TABLE_hacc6fcf3_0[__Vtableidx3];
    __Vtableidx1 = (0x7fU & vlSelf->inst);
    vlSelf->toplevel__DOT__riscv_core__DOT__pc_write_enable 
        = Vtoplevel__ConstPool__TABLE_h888b3f2b_0[__Vtableidx1];
    vlSelf->toplevel__DOT__riscv_core__DOT__regfile_write_enable 
        = Vtoplevel__ConstPool__TABLE_hcae45265_0[__Vtableidx1];
    toplevel__DOT__riscv_core__DOT__alu_operand_a_select 
        = Vtoplevel__ConstPool__TABLE_he0c5286b_0[__Vtableidx1];
    toplevel__DOT__riscv_core__DOT__alu_operand_b_select 
        = Vtoplevel__ConstPool__TABLE_h0f0752ae_0[__Vtableidx1];
    toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_op_type 
        = Vtoplevel__ConstPool__TABLE_hf7367149_0[__Vtableidx1];
    toplevel__DOT__riscv_core__DOT__read_enable = Vtoplevel__ConstPool__TABLE_h5c001984_0
        [__Vtableidx1];
    toplevel__DOT__riscv_core__DOT__write_enable = 
        Vtoplevel__ConstPool__TABLE_h5b40c0e4_0[__Vtableidx1];
    vlSelf->toplevel__DOT__riscv_core__DOT__reg_writeback_select 
        = Vtoplevel__ConstPool__TABLE_h7ffbd43f_0[__Vtableidx1];
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT____Vcellinp__multiplexer__in_bus 
        = (((QData)((IData)(vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register
                            [(0x1fU & (vlSelf->inst 
                                       >> 0xfU))])) 
            << 0x20U) | (QData)((IData)(vlSelf->pc)));
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__rs2_data 
        = vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register
        [(0x1fU & (vlSelf->inst >> 0x14U))];
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__immediate 
        = ((0x40U & vlSelf->inst) ? ((0x20U & vlSelf->inst)
                                      ? ((0x10U & vlSelf->inst)
                                          ? 0U : ((8U 
                                                   & vlSelf->inst)
                                                   ? 
                                                  ((4U 
                                                    & vlSelf->inst)
                                                    ? 
                                                   ((2U 
                                                     & vlSelf->inst)
                                                     ? 
                                                    ((1U 
                                                      & vlSelf->inst)
                                                      ? 
                                                     (((- (IData)(
                                                                  (vlSelf->inst 
                                                                   >> 0x1fU))) 
                                                       << 0x14U) 
                                                      | ((0xff000U 
                                                          & vlSelf->inst) 
                                                         | ((0x800U 
                                                             & (vlSelf->inst 
                                                                >> 9U)) 
                                                            | ((0x7e0U 
                                                                & (vlSelf->inst 
                                                                   >> 0x14U)) 
                                                               | (0x1eU 
                                                                  & (vlSelf->inst 
                                                                     >> 0x14U))))))
                                                      : 0U)
                                                     : 0U)
                                                    : 0U)
                                                   : 
                                                  ((4U 
                                                    & vlSelf->inst)
                                                    ? 
                                                   ((2U 
                                                     & vlSelf->inst)
                                                     ? 
                                                    ((1U 
                                                      & vlSelf->inst)
                                                      ? 
                                                     (((- (IData)(
                                                                  (vlSelf->inst 
                                                                   >> 0x1fU))) 
                                                       << 0xbU) 
                                                      | (0x7ffU 
                                                         & (vlSelf->inst 
                                                            >> 0x14U)))
                                                      : 0U)
                                                     : 0U)
                                                    : 
                                                   ((2U 
                                                     & vlSelf->inst)
                                                     ? 
                                                    ((1U 
                                                      & vlSelf->inst)
                                                      ? 
                                                     (((- (IData)(
                                                                  (vlSelf->inst 
                                                                   >> 0x1fU))) 
                                                       << 0xcU) 
                                                      | ((0x800U 
                                                          & (vlSelf->inst 
                                                             << 4U)) 
                                                         | ((0x7e0U 
                                                             & (vlSelf->inst 
                                                                >> 0x14U)) 
                                                            | (0x1eU 
                                                               & (vlSelf->inst 
                                                                  >> 7U)))))
                                                      : 0U)
                                                     : 0U))))
                                      : 0U) : ((0x20U 
                                                & vlSelf->inst)
                                                ? (
                                                   (0x10U 
                                                    & vlSelf->inst)
                                                    ? 
                                                   ((8U 
                                                     & vlSelf->inst)
                                                     ? 0U
                                                     : 
                                                    ((4U 
                                                      & vlSelf->inst)
                                                      ? 
                                                     ((2U 
                                                       & vlSelf->inst)
                                                       ? 
                                                      ((1U 
                                                        & vlSelf->inst)
                                                        ? 
                                                       (0xfffff000U 
                                                        & vlSelf->inst)
                                                        : 0U)
                                                       : 0U)
                                                      : 0U))
                                                    : 
                                                   ((8U 
                                                     & vlSelf->inst)
                                                     ? 0U
                                                     : 
                                                    ((2U 
                                                      & vlSelf->inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->inst)
                                                       ? 
                                                      (((- (IData)(
                                                                   (vlSelf->inst 
                                                                    >> 0x1fU))) 
                                                        << 0xbU) 
                                                       | ((0x7e0U 
                                                           & (vlSelf->inst 
                                                              >> 0x14U)) 
                                                          | (0x1fU 
                                                             & (vlSelf->inst 
                                                                >> 7U))))
                                                       : 0U)
                                                      : 0U)))
                                                : (
                                                   (0x10U 
                                                    & vlSelf->inst)
                                                    ? 
                                                   ((8U 
                                                     & vlSelf->inst)
                                                     ? 0U
                                                     : 
                                                    ((4U 
                                                      & vlSelf->inst)
                                                      ? 
                                                     ((2U 
                                                       & vlSelf->inst)
                                                       ? 
                                                      ((1U 
                                                        & vlSelf->inst)
                                                        ? 
                                                       (0xfffff000U 
                                                        & vlSelf->inst)
                                                        : 0U)
                                                       : 0U)
                                                      : 
                                                     ((2U 
                                                       & vlSelf->inst)
                                                       ? 
                                                      ((1U 
                                                        & vlSelf->inst)
                                                        ? 
                                                       (((- (IData)(
                                                                    (vlSelf->inst 
                                                                     >> 0x1fU))) 
                                                         << 0xbU) 
                                                        | (0x7ffU 
                                                           & (vlSelf->inst 
                                                              >> 0x14U)))
                                                        : 0U)
                                                       : 0U)))
                                                    : 
                                                   ((8U 
                                                     & vlSelf->inst)
                                                     ? 0U
                                                     : 
                                                    ((2U 
                                                      & vlSelf->inst)
                                                      ? 
                                                     ((1U 
                                                       & vlSelf->inst)
                                                       ? 
                                                      (((- (IData)(
                                                                   (vlSelf->inst 
                                                                    >> 0x1fU))) 
                                                        << 0xbU) 
                                                       | (0x7ffU 
                                                          & (vlSelf->inst 
                                                             >> 0x14U)))
                                                       : 0U)
                                                      : 0U)))));
    vlSelf->bus_write_enable = toplevel__DOT__riscv_core__DOT__write_enable;
    vlSelf->bus_read_enable = toplevel__DOT__riscv_core__DOT__read_enable;
    toplevel__DOT__riscv_core__DOT__alu_function = 
        ((2U & (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_op_type))
          ? ((1U & (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_op_type))
              ? ((IData)((0x40001000U == (0x40003000U 
                                          & vlSelf->inst)))
                  ? (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__secondary_funct)
                  : (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__default_funct))
              : ((0x40000000U & vlSelf->inst) ? (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__secondary_funct)
                  : (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_control__DOT__default_funct)))
          : ((1U & (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__alu_op_type))
              ? ((0x4000U & vlSelf->inst) ? ((0x2000U 
                                              & vlSelf->inst)
                                              ? 8U : 7U)
                  : ((0x2000U & vlSelf->inst) ? 0U : 6U))
              : 1U));
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT__multiplexer__DOT__input_array[1U] 
        = (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT____Vcellinp__multiplexer__in_bus);
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT__multiplexer__DOT__input_array[0U] 
        = (IData)((toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT____Vcellinp__multiplexer__in_bus 
                   >> 0x20U));
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT____Vcellinp__multiplexer__in_bus 
        = (((QData)((IData)(toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__rs2_data)) 
            << 0x20U) | (QData)((IData)(toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__immediate)));
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
        = vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT__multiplexer__DOT__input_array
        [toplevel__DOT__riscv_core__DOT__alu_operand_a_select];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT__multiplexer__DOT__input_array[1U] 
        = (IData)(toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT____Vcellinp__multiplexer__in_bus);
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT__multiplexer__DOT__input_array[0U] 
        = (IData)((toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT____Vcellinp__multiplexer__in_bus 
                   >> 0x20U));
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b 
        = vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT__multiplexer__DOT__input_array
        [toplevel__DOT__riscv_core__DOT__alu_operand_b_select];
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result 
        = ((0x10U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
            ? 0U : ((8U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                     ? ((4U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                         ? 0U : ((2U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                                  ? ((1U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                                      ? (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                         & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b)
                                      : (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                         | toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b))
                                  : ((1U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                                      ? (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                         ^ toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b)
                                      : (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                         < toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b))))
                     : ((4U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                         ? ((2U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                             ? ((1U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                                 ? VL_LTS_III(32, toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a, toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b)
                                 : (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                    == toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b))
                             : ((1U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                                 ? VL_SHIFTRS_III(32,32,5, toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a, 
                                                  (0x1fU 
                                                   & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b))
                                 : (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                    >> (0x1fU & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b))))
                         : ((2U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                             ? ((1U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                                 ? (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                    << (0x1fU & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b))
                                 : (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                    - toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b))
                             : ((1U & (IData)(toplevel__DOT__riscv_core__DOT__alu_function))
                                 ? (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_a 
                                    + toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_operand_b)
                                 : 0U)))));
    vlSelf->bus_write_data = ((0x1fU >= (0x18U & (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result 
                                                  << 3U)))
                               ? (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__rs2_data 
                                  << (0x18U & (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result 
                                               << 3U)))
                               : 0U);
    vlSelf->bus_byte_enable = 0U;
    vlSelf->bus_byte_enable = (0xfU & ((0U == (3U & 
                                               (vlSelf->inst 
                                                >> 0xcU)))
                                        ? ((IData)(1U) 
                                           << (3U & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result))
                                        : ((1U == (3U 
                                                   & (vlSelf->inst 
                                                      >> 0xcU)))
                                            ? ((IData)(3U) 
                                               << (3U 
                                                   & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result))
                                            : ((2U 
                                                == 
                                                (3U 
                                                 & (vlSelf->inst 
                                                    >> 0xcU)))
                                                ? ((IData)(0xfU) 
                                                   << 
                                                   (3U 
                                                    & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result))
                                                : 0U))));
    __Vtableidx2 = (((0U == toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result) 
                     << 3U) | (7U & (vlSelf->inst >> 0xcU)));
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__take_branch 
        = Vtoplevel__ConstPool__TABLE_hfd21f17e_0[__Vtableidx2];
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[0U] = 0U;
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[1U] 
        = (0xfffffffeU & toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result);
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[2U] 
        = (IData)((((QData)((IData)(((IData)(4U) + vlSelf->pc))) 
                    << 0x20U) | (QData)((IData)((vlSelf->pc 
                                                 + toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__immediate)))));
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[3U] 
        = (IData)(((((QData)((IData)(((IData)(4U) + vlSelf->pc))) 
                     << 0x20U) | (QData)((IData)((vlSelf->pc 
                                                  + toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__immediate)))) 
                   >> 0x20U));
    vlSelf->bus_address = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result;
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT__multiplexer__DOT__input_array[3U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[0U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT__multiplexer__DOT__input_array[2U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[1U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT__multiplexer__DOT__input_array[1U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[2U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT__multiplexer__DOT__input_array[0U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT____Vcellinp__multiplexer__in_bus[3U];
    vlSelf->toplevel__DOT__data_memory_bus__DOT__is_data_memory 
        = ((0x80000000U <= vlSelf->bus_address) & (0x8001ffffU 
                                                   >= vlSelf->bus_address));
    vlSelf->bus_read_data = (((IData)(vlSelf->bus_read_enable) 
                              & (IData)(vlSelf->toplevel__DOT__data_memory_bus__DOT__is_data_memory))
                              ? vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                             [(0x7fffU & (vlSelf->bus_address 
                                          >> 2U))] : 0U);
    toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix 
        = ((0x1fU >= (0x18U & (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result 
                               << 3U))) ? (vlSelf->bus_read_data 
                                           >> (0x18U 
                                               & (toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result 
                                                  << 3U)))
            : 0U);
    __Vtemp_hccccb3ad__0[1U] = (IData)((((QData)((IData)(toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result)) 
                                         << 0x20U) 
                                        | (QData)((IData)(
                                                          ((0U 
                                                            == 
                                                            (3U 
                                                             & (vlSelf->inst 
                                                                >> 0xcU)))
                                                            ? 
                                                           (((- (IData)(
                                                                        (1U 
                                                                         & ((~ 
                                                                             (vlSelf->inst 
                                                                              >> 0xeU)) 
                                                                            & (toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix 
                                                                               >> 7U))))) 
                                                             << 8U) 
                                                            | (0xffU 
                                                               & toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix))
                                                            : 
                                                           ((1U 
                                                             == 
                                                             (3U 
                                                              & (vlSelf->inst 
                                                                 >> 0xcU)))
                                                             ? 
                                                            (((- (IData)(
                                                                         (1U 
                                                                          & ((~ 
                                                                              (vlSelf->inst 
                                                                               >> 0xeU)) 
                                                                             & (toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix 
                                                                                >> 0xfU))))) 
                                                              << 0x10U) 
                                                             | (0xffffU 
                                                                & toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix))
                                                             : 
                                                            ((2U 
                                                              == 
                                                              (3U 
                                                               & (vlSelf->inst 
                                                                  >> 0xcU)))
                                                              ? toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix
                                                              : 0U)))))));
    __Vtemp_hccccb3ad__0[2U] = (IData)(((((QData)((IData)(toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__alu_result)) 
                                          << 0x20U) 
                                         | (QData)((IData)(
                                                           ((0U 
                                                             == 
                                                             (3U 
                                                              & (vlSelf->inst 
                                                                 >> 0xcU)))
                                                             ? 
                                                            (((- (IData)(
                                                                         (1U 
                                                                          & ((~ 
                                                                              (vlSelf->inst 
                                                                               >> 0xeU)) 
                                                                             & (toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix 
                                                                                >> 7U))))) 
                                                              << 8U) 
                                                             | (0xffU 
                                                                & toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix))
                                                             : 
                                                            ((1U 
                                                              == 
                                                              (3U 
                                                               & (vlSelf->inst 
                                                                  >> 0xcU)))
                                                              ? 
                                                             (((- (IData)(
                                                                          (1U 
                                                                           & ((~ 
                                                                               (vlSelf->inst 
                                                                                >> 0xeU)) 
                                                                              & (toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix 
                                                                                >> 0xfU))))) 
                                                               << 0x10U) 
                                                              | (0xffffU 
                                                                 & toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix))
                                                              : 
                                                             ((2U 
                                                               == 
                                                               (3U 
                                                                & (vlSelf->inst 
                                                                   >> 0xcU)))
                                                               ? toplevel__DOT__riscv_core__DOT__data_memory_interface__DOT__position_fix
                                                               : 0U)))))) 
                                        >> 0x20U));
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[0U] = 0U;
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[1U] = 0U;
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[2U] = 0U;
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[3U] = 0U;
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[4U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__immediate;
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[5U] 
        = ((IData)(4U) + vlSelf->pc);
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[6U] 
        = __Vtemp_hccccb3ad__0[1U];
    toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[7U] 
        = __Vtemp_hccccb3ad__0[2U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[7U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[0U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[6U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[1U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[5U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[2U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[4U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[3U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[3U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[4U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[2U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[5U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[1U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[6U];
    vlSelf->toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array[0U] 
        = toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT____Vcellinp__multiplexer__in_bus[7U];
}

void Vtoplevel___024root___eval(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval\n"); );
    // Body
    if (((IData)(vlSelf->clock) & (~ (IData)(vlSelf->__Vclklast__TOP__clock)))) {
        Vtoplevel___024root___sequent__TOP__0(vlSelf);
    }
    if ((((IData)(vlSelf->clock) & (~ (IData)(vlSelf->__Vclklast__TOP__clock))) 
         | ((IData)(vlSelf->reset) & (~ (IData)(vlSelf->__Vclklast__TOP__reset))))) {
        Vtoplevel___024root___sequent__TOP__1(vlSelf);
    }
    // Final
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
    vlSelf->__Vclklast__TOP__reset = vlSelf->reset;
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
