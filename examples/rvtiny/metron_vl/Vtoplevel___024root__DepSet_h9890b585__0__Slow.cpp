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
    vlSelf->pc = 0U;
    vlSelf->toplevel__DOT__regs[0U] = 0U;
    __Vtemp_hded71218__0[0U] = 0x653d2573U;
    __Vtemp_hded71218__0[1U] = 0x5f66696cU;
    __Vtemp_hded71218__0[2U] = 0x74657874U;
    (void)VL_VALUEPLUSARGS_INN(64, VL_CVT_PACK_STR_NW(3, __Vtemp_hded71218__0), 
                               vlSelf->toplevel__DOT__unnamedblk1__DOT__s);VL_READMEM_N(true
                                                                                , 32
                                                                                , 32768
                                                                                , 0
                                                                                , 
                                                                                VL_CVT_PACK_STR_NN(vlSelf->toplevel__DOT__unnamedblk1__DOT__s)
                                                                                ,  &(vlSelf->toplevel__DOT__text_mem)
                                                                                , 0
                                                                                , ~0ULL);
    __Vtemp_hfdc9732b__0[0U] = 0x653d2573U;
    __Vtemp_hfdc9732b__0[1U] = 0x5f66696cU;
    __Vtemp_hfdc9732b__0[2U] = 0x64617461U;
    (void)VL_VALUEPLUSARGS_INN(64, VL_CVT_PACK_STR_NW(3, __Vtemp_hfdc9732b__0), 
                               vlSelf->toplevel__DOT__unnamedblk1__DOT__s);VL_READMEM_N(true
                                                                                , 32
                                                                                , 32768
                                                                                , 0
                                                                                , 
                                                                                VL_CVT_PACK_STR_NN(vlSelf->toplevel__DOT__unnamedblk1__DOT__s)
                                                                                ,  &(vlSelf->toplevel__DOT__data_mem)
                                                                                , 0
                                                                                , ~0ULL);
}

VL_ATTR_COLD void Vtoplevel___024root___eval_initial(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_initial\n"); );
    // Body
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
    Vtoplevel___024root___initial__TOP__0(vlSelf);
}

VL_ATTR_COLD void Vtoplevel___024root___eval_settle(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_settle\n"); );
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
    vlSelf->bus_read_data = VL_RAND_RESET_I(32);
    vlSelf->bus_address = VL_RAND_RESET_I(32);
    vlSelf->bus_write_data = VL_RAND_RESET_I(32);
    vlSelf->bus_byte_enable = VL_RAND_RESET_I(4);
    vlSelf->bus_read_enable = VL_RAND_RESET_I(1);
    vlSelf->bus_write_enable = VL_RAND_RESET_I(1);
    vlSelf->inst = VL_RAND_RESET_I(32);
    vlSelf->pc = VL_RAND_RESET_I(32);
    vlSelf->tock_reset = VL_RAND_RESET_I(1);
    for (int __Vi0=0; __Vi0<32768; ++__Vi0) {
        vlSelf->toplevel__DOT__text_mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    for (int __Vi0=0; __Vi0<32768; ++__Vi0) {
        vlSelf->toplevel__DOT__data_mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    for (int __Vi0=0; __Vi0<32; ++__Vi0) {
        vlSelf->toplevel__DOT__regs[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__inst = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__op = VL_RAND_RESET_I(7);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__rd = VL_RAND_RESET_I(5);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__f3 = VL_RAND_RESET_I(3);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__r1 = VL_RAND_RESET_I(5);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__r2 = VL_RAND_RESET_I(5);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__f7 = VL_RAND_RESET_I(7);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk3__DOT__alu_result = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk4__DOT__imm = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk4__DOT__addr = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk5__DOT__imm = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk5__DOT__data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk5__DOT__mask = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk5__DOT__phys_addr = VL_RAND_RESET_I(15);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk6__DOT__take_branch = VL_RAND_RESET_I(1);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk6__DOT__unnamedblk7__DOT__imm = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk8__DOT__imm = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk9__DOT__rr1 = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk9__DOT__imm = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk10__DOT__imm = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__tick__DOT__unnamedblk2__DOT__unnamedblk11__DOT__imm = VL_RAND_RESET_I(32);
}
