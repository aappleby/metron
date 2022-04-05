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
    IData/*31:0*/ __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask;
    // Body
    if ((((IData)(vlSelf->toplevel__DOT__data_memory_bus_write_enable) 
          & (0x80000000U <= vlSelf->toplevel__DOT__data_memory_bus_address)) 
         & (0x8001ffffU >= vlSelf->toplevel__DOT__data_memory_bus_address))) {
        __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask = 0U;
        if ((1U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xffU | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        if ((2U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xff00U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        if ((4U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xff0000U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        if ((8U & (IData)(vlSelf->toplevel__DOT__data_memory_bus_byte_enable))) {
            __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask 
                = (0xff000000U | __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask);
        }
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address] 
            = ((vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
                [vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory_address] 
                & (~ __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask)) 
               | (vlSelf->toplevel__DOT__data_memory_bus_write_data 
                  & __Vtask_toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__tick__2__unnamedblk2__DOT__mask));
    }
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
    if (vlSelf->reset) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value = 0x400000U;
    } else if (vlSelf->toplevel__DOT__core__DOT__datapath_pc_write_enable) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value 
            = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_next;
    }
    toplevel__DOT__unnamedblk1__DOT__read_data = toplevel__DOT__data_memory_bus_read_data;
    vlSelf->o_bus_read_data = toplevel__DOT__unnamedblk1__DOT__read_data;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_4 
        = ((IData)(4U) + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value);
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk3__DOT__pc_plus_4;
    toplevel__DOT__unnamedblk1__DOT__pc = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value;
    toplevel__DOT__text_memory_bus_address = toplevel__DOT__unnamedblk1__DOT__pc;
    vlSelf->o_pc = toplevel__DOT__unnamedblk1__DOT__pc;
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
