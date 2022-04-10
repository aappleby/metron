// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel___024root.h"

VL_ATTR_COLD void Vtoplevel___024root___initial__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___initial__TOP__0\n"); );
    // Init
    VlWide<3>/*95:0*/ __Vtemp_hded71218__0;
    VlWide<3>/*95:0*/ __Vtemp_hfdc9732b__0;
    // Body
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register[0U] = 0U;
    __Vtemp_hded71218__0[0U] = 0x653d2573U;
    __Vtemp_hded71218__0[1U] = 0x5f66696cU;
    __Vtemp_hded71218__0[2U] = 0x74657874U;
    (void)VL_VALUEPLUSARGS_INN(64, VL_CVT_PACK_STR_NW(3, __Vtemp_hded71218__0), 
                               vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__unnamedblk1__DOT__s);VL_READMEM_N(true
                                                                                , 32
                                                                                , 16384
                                                                                , 0
                                                                                , 
                                                                                VL_CVT_PACK_STR_NN(vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__unnamedblk1__DOT__s)
                                                                                ,  &(vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem)
                                                                                , 0
                                                                                , ~0ULL);
    __Vtemp_hfdc9732b__0[0U] = 0x653d2573U;
    __Vtemp_hfdc9732b__0[1U] = 0x5f66696cU;
    __Vtemp_hfdc9732b__0[2U] = 0x64617461U;
    (void)VL_VALUEPLUSARGS_INN(64, VL_CVT_PACK_STR_NW(3, __Vtemp_hfdc9732b__0), 
                               vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__unnamedblk1__DOT__s);VL_READMEM_N(true
                                                                                , 32
                                                                                , 32768
                                                                                , 0
                                                                                , 
                                                                                VL_CVT_PACK_STR_NN(vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__unnamedblk1__DOT__s)
                                                                                ,  &(vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem)
                                                                                , 0
                                                                                , ~0ULL);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value = 0x400000U;
}

extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_hcae45265_0;
extern const VlUnpacked<CData/*2:0*/, 128> Vtoplevel__ConstPool__TABLE_h7ffbd43f_0;
extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_h0f0752ae_0;
extern const VlUnpacked<CData/*1:0*/, 128> Vtoplevel__ConstPool__TABLE_hf7367149_0;
extern const VlUnpacked<CData/*0:0*/, 16> Vtoplevel__ConstPool__TABLE_hfd21f17e_0;
extern const VlUnpacked<CData/*1:0*/, 256> Vtoplevel__ConstPool__TABLE_h595af980_0;

VL_ATTR_COLD void Vtoplevel___024root___settle__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___settle__TOP__0\n"); );
    // Init
    IData/*31:0*/ toplevel__DOT__data_memory_bus_q;
    CData/*0:0*/ toplevel__DOT__core__DOT__datapath__DOT__alu_core_result_equal_zero;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
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
    CData/*7:0*/ __Vtableidx1;
    CData/*6:0*/ __Vtableidx2;
    CData/*6:0*/ __Vtableidx3;
    CData/*6:0*/ __Vtableidx4;
    CData/*6:0*/ __Vtableidx5;
    CData/*3:0*/ __Vtableidx6;
    // Body
    vlSelf->pc = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value;
    toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr 
        = ((0x400000U <= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value) 
           & (0x40ffffU >= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value));
    vlSelf->toplevel__DOT__text_memory_bus_read_data 
        = ((IData)(toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr)
            ? vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem
           [(0x3fffU & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value 
                        >> 2U))] : 0U);
    vlSelf->bus_read_enable = (3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data));
    vlSelf->bus_write_enable = (0x23U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data));
    vlSelf->inst = vlSelf->toplevel__DOT__text_memory_bus_read_data;
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
        = (((((3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
              | (7U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
             | (0x13U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
            | (0x67U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)))
            ? (((- (IData)((vlSelf->toplevel__DOT__text_memory_bus_read_data 
                            >> 0x1fU))) << 0xbU) | 
               (0x7ffU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                          >> 0x14U))) : (((0x27U == 
                                           (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
                                          | (0x23U 
                                             == (0x7fU 
                                                 & vlSelf->toplevel__DOT__text_memory_bus_read_data)))
                                          ? (((- (IData)(
                                                         (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                          >> 0x1fU))) 
                                              << 0xbU) 
                                             | ((0x7e0U 
                                                 & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                    >> 0x14U)) 
                                                | (0x1fU 
                                                   & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                      >> 7U))))
                                          : ((0x63U 
                                              == (0x7fU 
                                                  & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                              ? (((- (IData)(
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
                                              : (((0x17U 
                                                   == 
                                                   (0x7fU 
                                                    & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
                                                  | (0x37U 
                                                     == 
                                                     (0x7fU 
                                                      & vlSelf->toplevel__DOT__text_memory_bus_read_data)))
                                                  ? 
                                                 (0xfffff000U 
                                                  & vlSelf->toplevel__DOT__text_memory_bus_read_data)
                                                  : 
                                                 ((0x6fU 
                                                   == 
                                                   (0x7fU 
                                                    & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                   ? 
                                                  (((- (IData)(
                                                               (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                >> 0x1fU))) 
                                                    << 0x14U) 
                                                   | ((0xff000U 
                                                       & vlSelf->toplevel__DOT__text_memory_bus_read_data) 
                                                      | ((0x800U 
                                                          & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                             >> 9U)) 
                                                         | ((0x7e0U 
                                                             & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                >> 0x14U)) 
                                                            | (0x1eU 
                                                               & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                  >> 0x14U))))))
                                                   : 0U)))));
    toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data 
        = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
        [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                   >> 0x14U))];
    __Vtableidx4 = (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data);
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type 
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
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type))
            ? 1U : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type))
                     ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_funct)
                     : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type))
                         ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_imm_funct)
                         : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type))
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
                         : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                             : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                                 : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                                     : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                                                  vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                                        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                                                      vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                                             : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                         : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                             : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
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
                                 : 0U) : vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register
                            [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                       >> 0xfU))]) 
                           & ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                               ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                                   ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                                   : 0U) : toplevel__DOT__core__DOT__datapath__DOT__regs_rs2_data))
                        : 0U))));
    toplevel__DOT__core__DOT__datapath__DOT__alu_core_result_equal_zero 
        = (0U == vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result);
    __Vtableidx6 = (((IData)(toplevel__DOT__core__DOT__datapath__DOT__alu_core_result_equal_zero) 
                     << 3U) | (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                     >> 0xcU)));
    toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch 
        = Vtoplevel__ConstPool__TABLE_hfd21f17e_0[__Vtableidx6];
    vlSelf->bus_address = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result;
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
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory 
        = ((0x80000000U <= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result) 
           & (0x8001ffffU >= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result));
    toplevel__DOT__data_memory_bus_q = (((3U == (0x7fU 
                                                 & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
                                         & (IData)(toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory))
                                         ? vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                                        [(0x7fffU & 
                                          (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
                                           >> 2U))]
                                         : 0U);
    __Vtableidx1 = (((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch) 
                     << 7U) | (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data));
    toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select 
        = Vtoplevel__ConstPool__TABLE_h595af980_0[__Vtableidx1];
    vlSelf->bus_write_data = vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data;
    vlSelf->bus_byte_enable = vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable;
    vlSelf->bus_read_data = toplevel__DOT__data_memory_bus_q;
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

VL_ATTR_COLD void Vtoplevel___024root___eval_initial(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_initial\n"); );
    // Body
    Vtoplevel___024root___initial__TOP__0(vlSelf);
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
}

VL_ATTR_COLD void Vtoplevel___024root___eval_settle(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_settle\n"); );
    // Body
    Vtoplevel___024root___settle__TOP__0(vlSelf);
}

VL_ATTR_COLD void Vtoplevel___024root___final(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___final\n"); );
}

VL_ATTR_COLD void Vtoplevel___024root___ctor_var_reset(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clock = VL_RAND_RESET_I(1);
    vlSelf->reset = VL_RAND_RESET_I(1);
    vlSelf->bus_read_data = VL_RAND_RESET_I(32);
    vlSelf->bus_address = VL_RAND_RESET_I(32);
    vlSelf->bus_write_data = VL_RAND_RESET_I(32);
    vlSelf->bus_byte_enable = VL_RAND_RESET_I(4);
    vlSelf->bus_read_enable = VL_RAND_RESET_I(1);
    vlSelf->bus_write_enable = VL_RAND_RESET_I(1);
    vlSelf->inst = VL_RAND_RESET_I(32);
    vlSelf->pc = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__text_memory_bus_read_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__dmem_read_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable = VL_RAND_RESET_I(4);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_result = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_value = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__igen_immediate = VL_RAND_RESET_I(32);
    for (int __Vi0=0; __Vi0<32; ++__Vi0) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT___register[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable = VL_RAND_RESET_I(1);
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select = VL_RAND_RESET_I(3);
    for (int __Vi0=0; __Vi0<16384; ++__Vi0) {
        vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    for (int __Vi0=0; __Vi0<32768; ++__Vi0) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    }
