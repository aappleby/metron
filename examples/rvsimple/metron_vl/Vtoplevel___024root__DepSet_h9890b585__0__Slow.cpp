// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel___024root.h"

VL_ATTR_COLD void Vtoplevel___024root___settle__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___settle__TOP__0\n"); );
    // Init
    IData/*31:0*/ toplevel__DOT__unnamedblk1__DOT__alu_result2;
    IData/*31:0*/ toplevel__DOT__unnamedblk1__DOT__write_data;
    CData/*0:0*/ toplevel__DOT__unnamedblk1__DOT__write_enable;
    CData/*0:0*/ toplevel__DOT__unnamedblk1__DOT__read_enable;
    CData/*1:0*/ toplevel__DOT__core__DOT__unnamedblk5__DOT__pc_select;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__alu_core__DOT__unnamedblk1__DOT__result;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk1__DOT__result;
    // Body
    toplevel__DOT__unnamedblk1__DOT__read_enable = 
        (3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode));
    vlSelf->toplevel__DOT__data_memory_bus_read_enable 
        = toplevel__DOT__unnamedblk1__DOT__read_enable;
    vlSelf->o_bus_read_enable = toplevel__DOT__unnamedblk1__DOT__read_enable;
    toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk1__DOT__result 
        = (3U & ((0x63U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))
                  ? ((IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch)
                      ? 1U : 0U) : ((0x67U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))
                                     ? 2U : ((0x6fU 
                                              == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode))
                                              ? 1U : 0U))));
    toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__datapath__DOT__alu_core__DOT__unnamedblk1__DOT__result 
        = (((((((((1U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function)) 
                  | (2U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))) 
                 | (3U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))) 
                | (4U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))) 
               | (5U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))) 
              | (6U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))) 
             | (7U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))) 
            | (8U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function)))
            ? ((1U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                   + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b)
                : ((2U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                    ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                       - vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b)
                    : ((3U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                        ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                           << (0x1fU & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b))
                        : ((4U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                            ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                               >> (0x1fU & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b))
                            : ((5U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                                ? VL_SHIFTRS_III(32,32,5, vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a, 
                                                 (0x1fU 
                                                  & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b))
                                : ((6U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                                    ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                                       == vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b)
                                    : ((7U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                                        ? VL_LTS_III(32, vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a, vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b)
                                        : (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                                           < vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b))))))))
            : ((9U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                   ^ vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b)
                : ((0xaU == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                    ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                       | vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b)
                    : ((0xbU == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function))
                        ? (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a 
                           & vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b)
                        : 0U))));
    toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_result 
        = toplevel__DOT__core__DOT__datapath__DOT__alu_core__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__unnamedblk1__DOT__write_enable = 
        (0x23U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode));
    vlSelf->o_bus_write_enable = toplevel__DOT__unnamedblk1__DOT__write_enable;
    vlSelf->toplevel__DOT__data_memory_bus_write_enable 
        = toplevel__DOT__unnamedblk1__DOT__write_enable;
    vlSelf->toplevel__DOT__core__DOT__datapath_pc_write_enable = 1U;
    toplevel__DOT__core__DOT__unnamedblk5__DOT__pc_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
    toplevel__DOT__core__DOT__unnamedblk5__DOT__pc_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
    vlSelf->toplevel__DOT__core__DOT__datapath_next_pc_select 
        = toplevel__DOT__core__DOT__unnamedblk5__DOT__pc_select;
    toplevel__DOT__unnamedblk1__DOT__alu_result2 = toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_result;
    vlSelf->o_bus_address = toplevel__DOT__unnamedblk1__DOT__alu_result2;
    toplevel__DOT__unnamedblk1__DOT__alu_result2 = toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_result;
    vlSelf->o_bus_address = toplevel__DOT__unnamedblk1__DOT__alu_result2;
    vlSelf->toplevel__DOT__data_memory_bus_address 
        = toplevel__DOT__unnamedblk1__DOT__alu_result2;
    vlSelf->toplevel__DOT__core_alu_result2 = toplevel__DOT__unnamedblk1__DOT__alu_result2;
    if ((0x1fU >= (0x18U & (vlSelf->toplevel__DOT__core_alu_result2 
                            << 3U)))) {
        toplevel__DOT__unnamedblk1__DOT__write_data 
            = (vlSelf->toplevel__DOT__core__DOT__dmem_write_data 
               << (0x18U & (vlSelf->toplevel__DOT__core_alu_result2 
                            << 3U)));
        vlSelf->o_bus_write_data = toplevel__DOT__unnamedblk1__DOT__write_data;
        toplevel__DOT__unnamedblk1__DOT__write_data 
            = (vlSelf->toplevel__DOT__core__DOT__dmem_write_data 
               << (0x18U & (vlSelf->toplevel__DOT__core_alu_result2 
                            << 3U)));
    } else {
        toplevel__DOT__unnamedblk1__DOT__write_data = 0U;
        vlSelf->o_bus_write_data = toplevel__DOT__unnamedblk1__DOT__write_data;
        toplevel__DOT__unnamedblk1__DOT__write_data = 0U;
    }
    vlSelf->o_bus_write_data = toplevel__DOT__unnamedblk1__DOT__write_data;
    vlSelf->toplevel__DOT__data_memory_bus_write_data 
        = toplevel__DOT__unnamedblk1__DOT__write_data;
    vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address 
        = (0x7fffU & (vlSelf->toplevel__DOT__data_memory_bus_address 
                      >> 2U));
}

VL_ATTR_COLD void Vtoplevel___024root___initial__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___initial__TOP__0\n"); );
    // Init
    VlWide<3>/*95:0*/ __Vtemp_hded71218__0;
    VlWide<3>/*95:0*/ __Vtemp_hfdc9732b__0;
    // Body
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
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value = 0x400000U;
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
}

VL_ATTR_COLD void Vtoplevel___024root___settle__TOP__1(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___settle__TOP__1\n"); );
    // Init
    IData/*31:0*/ toplevel__DOT__core_inst;
    IData/*31:0*/ toplevel__DOT__text_memory_bus_address;
    IData/*31:0*/ toplevel__DOT__text_memory_bus_read_data;
    IData/*31:0*/ toplevel__DOT__data_memory_bus_read_data;
    IData/*31:0*/ toplevel__DOT__unnamedblk1__DOT__pc;
    IData/*31:0*/ toplevel__DOT__unnamedblk1__DOT__inst;
    CData/*3:0*/ toplevel__DOT__unnamedblk1__DOT__byte_enable;
    IData/*31:0*/ toplevel__DOT__unnamedblk1__DOT__read_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath_inst;
    CData/*2:0*/ toplevel__DOT__core__DOT__dmem_data_format;
    CData/*3:0*/ toplevel__DOT__core__DOT__dmem_bus_byte_enable;
    CData/*2:0*/ toplevel__DOT__core__DOT__unnamedblk5__DOT__funct3;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__idec_inst;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__igen_immediate;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_4;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_imm;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select__DOT__unnamedblk1__DOT__result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__igen__DOT__unnamedblk1__DOT__result;
    CData/*3:0*/ toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__result;
    IData/*31:0*/ toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__fetched;
    CData/*0:0*/ toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr;
    IData/*31:0*/ toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched;
    CData/*0:0*/ toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory;
    // Body
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_4 
        = ((IData)(4U) + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value);
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_4;
    toplevel__DOT__unnamedblk1__DOT__pc = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value;
    toplevel__DOT__text_memory_bus_address = toplevel__DOT__unnamedblk1__DOT__pc;
    vlSelf->o_pc = toplevel__DOT__unnamedblk1__DOT__pc;
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched 
        = vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
        [vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address];
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory 
        = ((0x80000000U <= vlSelf->toplevel__DOT__data_memory_bus_address) 
           & (0x8001ffffU >= vlSelf->toplevel__DOT__data_memory_bus_address));
    toplevel__DOT__data_memory_bus_read_data = (((IData)(vlSelf->toplevel__DOT__data_memory_bus_read_enable) 
                                                 & (IData)(toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory))
                                                 ? toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched
                                                 : 0U);
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
    toplevel__DOT__unnamedblk1__DOT__read_data = toplevel__DOT__data_memory_bus_read_data;
    vlSelf->o_bus_read_data = toplevel__DOT__unnamedblk1__DOT__read_data;
    toplevel__DOT__unnamedblk1__DOT__inst = toplevel__DOT__text_memory_bus_read_data;
    vlSelf->o_inst = toplevel__DOT__unnamedblk1__DOT__inst;
    toplevel__DOT__core_inst = toplevel__DOT__unnamedblk1__DOT__inst;
    toplevel__DOT__core__DOT__datapath_inst = toplevel__DOT__core_inst;
    toplevel__DOT__core__DOT__datapath__DOT__igen__DOT__unnamedblk1__DOT__result 
        = (((((((((3U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst)) 
                  | (7U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))) 
                 | (0x13U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))) 
                | (0x67U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))) 
               | (0x27U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))) 
              | (0x23U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))) 
             | (0x63U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))) 
            | (0x17U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst)))
            ? ((3U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))
                ? (((- (IData)((toplevel__DOT__core__DOT__datapath_inst 
                                >> 0x1fU))) << 0xbU) 
                   | (0x7ffU & (toplevel__DOT__core__DOT__datapath_inst 
                                >> 0x14U))) : ((7U 
                                                == 
                                                (0x7fU 
                                                 & toplevel__DOT__core__DOT__datapath_inst))
                                                ? (
                                                   ((- (IData)(
                                                               (toplevel__DOT__core__DOT__datapath_inst 
                                                                >> 0x1fU))) 
                                                    << 0xbU) 
                                                   | (0x7ffU 
                                                      & (toplevel__DOT__core__DOT__datapath_inst 
                                                         >> 0x14U)))
                                                : (
                                                   (0x13U 
                                                    == 
                                                    (0x7fU 
                                                     & toplevel__DOT__core__DOT__datapath_inst))
                                                    ? 
                                                   (((- (IData)(
                                                                (toplevel__DOT__core__DOT__datapath_inst 
                                                                 >> 0x1fU))) 
                                                     << 0xbU) 
                                                    | (0x7ffU 
                                                       & (toplevel__DOT__core__DOT__datapath_inst 
                                                          >> 0x14U)))
                                                    : 
                                                   ((0x67U 
                                                     == 
                                                     (0x7fU 
                                                      & toplevel__DOT__core__DOT__datapath_inst))
                                                     ? 
                                                    (((- (IData)(
                                                                 (toplevel__DOT__core__DOT__datapath_inst 
                                                                  >> 0x1fU))) 
                                                      << 0xbU) 
                                                     | (0x7ffU 
                                                        & (toplevel__DOT__core__DOT__datapath_inst 
                                                           >> 0x14U)))
                                                     : 
                                                    ((0x27U 
                                                      == 
                                                      (0x7fU 
                                                       & toplevel__DOT__core__DOT__datapath_inst))
                                                      ? 
                                                     (((- (IData)(
                                                                  (toplevel__DOT__core__DOT__datapath_inst 
                                                                   >> 0x1fU))) 
                                                       << 0xbU) 
                                                      | ((0x7e0U 
                                                          & (toplevel__DOT__core__DOT__datapath_inst 
                                                             >> 0x14U)) 
                                                         | (0x1fU 
                                                            & (toplevel__DOT__core__DOT__datapath_inst 
                                                               >> 7U))))
                                                      : 
                                                     ((0x23U 
                                                       == 
                                                       (0x7fU 
                                                        & toplevel__DOT__core__DOT__datapath_inst))
                                                       ? 
                                                      (((- (IData)(
                                                                   (toplevel__DOT__core__DOT__datapath_inst 
                                                                    >> 0x1fU))) 
                                                        << 0xbU) 
                                                       | ((0x7e0U 
                                                           & (toplevel__DOT__core__DOT__datapath_inst 
                                                              >> 0x14U)) 
                                                          | (0x1fU 
                                                             & (toplevel__DOT__core__DOT__datapath_inst 
                                                                >> 7U))))
                                                       : 
                                                      ((0x63U 
                                                        == 
                                                        (0x7fU 
                                                         & toplevel__DOT__core__DOT__datapath_inst))
                                                        ? 
                                                       (((- (IData)(
                                                                    (toplevel__DOT__core__DOT__datapath_inst 
                                                                     >> 0x1fU))) 
                                                         << 0xcU) 
                                                        | ((0x800U 
                                                            & (toplevel__DOT__core__DOT__datapath_inst 
                                                               << 4U)) 
                                                           | ((0x7e0U 
                                                               & (toplevel__DOT__core__DOT__datapath_inst 
                                                                  >> 0x14U)) 
                                                              | (0x1eU 
                                                                 & (toplevel__DOT__core__DOT__datapath_inst 
                                                                    >> 7U)))))
                                                        : 
                                                       (0xfffff000U 
                                                        & toplevel__DOT__core__DOT__datapath_inst))))))))
            : ((0x37U == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))
                ? (0xfffff000U & toplevel__DOT__core__DOT__datapath_inst)
                : ((0x6fU == (0x7fU & toplevel__DOT__core__DOT__datapath_inst))
                    ? (((- (IData)((toplevel__DOT__core__DOT__datapath_inst 
                                    >> 0x1fU))) << 0x14U) 
                       | ((0xff000U & toplevel__DOT__core__DOT__datapath_inst) 
                          | ((0x800U & (toplevel__DOT__core__DOT__datapath_inst 
                                        >> 9U)) | (
                                                   (0x7e0U 
                                                    & (toplevel__DOT__core__DOT__datapath_inst 
                                                       >> 0x14U)) 
                                                   | (0x1eU 
                                                      & (toplevel__DOT__core__DOT__datapath_inst 
                                                         >> 0x14U))))))
                    : 0U)));
    toplevel__DOT__core__DOT__datapath__DOT__igen_immediate 
        = toplevel__DOT__core__DOT__datapath__DOT__igen__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
        = toplevel__DOT__core__DOT__datapath_inst;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_imm 
        = (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value 
           + toplevel__DOT__core__DOT__datapath__DOT__igen_immediate);
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_imm;
    toplevel__DOT__core__DOT__unnamedblk5__DOT__funct3 
        = (7U & (toplevel__DOT__core__DOT__datapath__DOT__idec_inst 
                 >> 0xcU));
    toplevel__DOT__core__DOT__dmem_data_format = toplevel__DOT__core__DOT__unnamedblk5__DOT__funct3;
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select__DOT__unnamedblk1__DOT__result 
        = ((0U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath_next_pc_select))
            ? toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0
            : ((1U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath_next_pc_select))
                ? toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1
                : ((2U == (IData)(vlSelf->toplevel__DOT__core__DOT__datapath_next_pc_select))
                    ? (0xfffffffeU & vlSelf->toplevel__DOT__core_alu_result2)
                    : 0U)));
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__result 
        = (0xfU & ((0U == (3U & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                    ? ((IData)(1U) << (3U & vlSelf->toplevel__DOT__core_alu_result2))
                    : ((1U == (3U & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                        ? ((IData)(3U) << (3U & vlSelf->toplevel__DOT__core_alu_result2))
                        : ((2U == (3U & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                            ? ((IData)(0xfU) << (3U 
                                                 & vlSelf->toplevel__DOT__core_alu_result2))
                            : 0U))));
    toplevel__DOT__core__DOT__dmem_bus_byte_enable 
        = toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_data 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out;
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_next 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_data;
    toplevel__DOT__unnamedblk1__DOT__byte_enable = toplevel__DOT__core__DOT__dmem_bus_byte_enable;
    vlSelf->o_bus_byte_enable = toplevel__DOT__unnamedblk1__DOT__byte_enable;
    vlSelf->toplevel__DOT__data_memory_bus_byte_enable 
        = toplevel__DOT__unnamedblk1__DOT__byte_enable;
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
    Vtoplevel___024root___settle__TOP__1(vlSelf);
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
    vlSelf->o_bus_read_data = VL_RAND_RESET_I(32);
    vlSelf->o_bus_address = VL_RAND_RESET_I(32);
    vlSelf->o_bus_write_data = VL_RAND_RESET_I(32);
    vlSelf->o_bus_byte_enable = VL_RAND_RESET_I(4);
    vlSelf->o_bus_read_enable = VL_RAND_RESET_I(1);
    vlSelf->o_bus_write_enable = VL_RAND_RESET_I(1);
    vlSelf->o_inst = VL_RAND_RESET_I(32);
    vlSelf->o_pc = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core_alu_result2 = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__data_memory_bus_address = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__data_memory_bus_read_enable = VL_RAND_RESET_I(1);
    vlSelf->toplevel__DOT__data_memory_bus_write_enable = VL_RAND_RESET_I(1);
    vlSelf->toplevel__DOT__data_memory_bus_byte_enable = VL_RAND_RESET_I(4);
    vlSelf->toplevel__DOT__data_memory_bus_write_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath_next_pc_select = VL_RAND_RESET_I(2);
    vlSelf->toplevel__DOT__core__DOT__datapath_pc_write_enable = VL_RAND_RESET_I(1);
    vlSelf->toplevel__DOT__core__DOT__dmem_write_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_alu_function = VL_RAND_RESET_I(5);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_a = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__alu_core_operand_b = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_next = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_inst_opcode = VL_RAND_RESET_I(7);
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch = VL_RAND_RESET_I(1);
    for (int __Vi0=0; __Vi0<16384; ++__Vi0) {
        vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address = VL_RAND_RESET_I(15);
    for (int __Vi0=0; __Vi0<32768; ++__Vi0) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    }
