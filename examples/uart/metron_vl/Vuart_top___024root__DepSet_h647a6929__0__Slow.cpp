// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vuart_top.h for the primary calling header

#include "verilated.h"

#include "Vuart_top___024root.h"

VL_ATTR_COLD void Vuart_top___024root___initial__TOP__0(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___initial__TOP__0\n"); );
    // Init
    VlWide<7>/*223:0*/ __Vtemp_hb501911b__0;
    // Body
    VL_WRITEF("uart_top.init()\n");
    __Vtemp_hb501911b__0[0U] = 0x2e686578U;
    __Vtemp_hb501911b__0[1U] = 0x73616765U;
    __Vtemp_hb501911b__0[2U] = 0x2f6d6573U;
    __Vtemp_hb501911b__0[3U] = 0x75617274U;
    __Vtemp_hb501911b__0[4U] = 0x6c65732fU;
    __Vtemp_hb501911b__0[5U] = 0x78616d70U;
    __Vtemp_hb501911b__0[6U] = 0x65U;
    VL_READMEM_N(true, 8, 512, 0, VL_CVT_PACK_STR_NW(7, __Vtemp_hb501911b__0)
                 ,  &(vlSelf->uart_top__DOT__hello__DOT__memory)
                 , 0U, 0x1ffU);
}

VL_ATTR_COLD void Vuart_top___024root___settle__TOP__0(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___settle__TOP__0\n"); );
    // Init
    CData/*7:0*/ uart_top__DOT__tock__DOT__hello_data;
    CData/*0:0*/ uart_top__DOT__tock__DOT__hello_req;
    // Body
    vlSelf->o_data = vlSelf->uart_top__DOT__rx__DOT__buffer;
    vlSelf->o_valid = (1U == (IData)(vlSelf->uart_top__DOT__rx__DOT__cursor));
    vlSelf->o_sum = vlSelf->uart_top__DOT__rx__DOT__sum;
    uart_top__DOT__tock__DOT__hello_data = vlSelf->uart_top__DOT__hello__DOT__data;
    vlSelf->uart_top__DOT__tx_i_data = uart_top__DOT__tock__DOT__hello_data;
    uart_top__DOT__tock__DOT__hello_req = (1U == (IData)(vlSelf->uart_top__DOT__hello__DOT__s));
    vlSelf->uart_top__DOT__tx_i_req = uart_top__DOT__tock__DOT__hello_req;
    vlSelf->uart_top__DOT__tx_o_idle = ((0U == (IData)(vlSelf->uart_top__DOT__tx__DOT__cursor)) 
                                        & (0U == (IData)(vlSelf->uart_top__DOT__tx__DOT__cycle)));
    vlSelf->o_done = ((2U == (IData)(vlSelf->uart_top__DOT__hello__DOT__s)) 
                      & (IData)(vlSelf->uart_top__DOT__tx_o_idle));
}

VL_ATTR_COLD void Vuart_top___024root___eval_initial(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___eval_initial\n"); );
    // Body
    Vuart_top___024root___initial__TOP__0(vlSelf);
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
}

VL_ATTR_COLD void Vuart_top___024root___eval_settle(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___eval_settle\n"); );
    // Body
    Vuart_top___024root___settle__TOP__0(vlSelf);
}

VL_ATTR_COLD void Vuart_top___024root___final(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___final\n"); );
}

VL_ATTR_COLD void Vuart_top___024root___ctor_var_reset(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clock = VL_RAND_RESET_I(1);
    vlSelf->i_rstn = VL_RAND_RESET_I(1);
    vlSelf->o_data = VL_RAND_RESET_I(8);
    vlSelf->o_valid = VL_RAND_RESET_I(1);
    vlSelf->o_done = VL_RAND_RESET_I(1);
    vlSelf->o_sum = VL_RAND_RESET_I(32);
    vlSelf->uart_top__DOT__tx_i_data = VL_RAND_RESET_I(8);
    vlSelf->uart_top__DOT__tx_i_req = VL_RAND_RESET_I(1);
    vlSelf->uart_top__DOT__tx_o_idle = VL_RAND_RESET_I(1);
    vlSelf->uart_top__DOT__hello__DOT__s = VL_RAND_RESET_I(2);
    vlSelf->uart_top__DOT__hello__DOT__cursor = VL_RAND_RESET_I(9);
    for (int __Vi0=0; __Vi0<512; ++__Vi0) {
        vlSelf->uart_top__DOT__hello__DOT__memory[__Vi0] = VL_RAND_RESET_I(8);
    }
    vlSelf->uart_top__DOT__hello__DOT__data = VL_RAND_RESET_I(8);
    vlSelf->uart_top__DOT__tx__DOT__cycle = VL_RAND_RESET_I(2);
    vlSelf->uart_top__DOT__tx__DOT__cursor = VL_RAND_RESET_I(5);
    vlSelf->uart_top__DOT__tx__DOT__buffer = VL_RAND_RESET_I(9);
    vlSelf->uart_top__DOT__rx__DOT__cycle = VL_RAND_RESET_I(2);
    vlSelf->uart_top__DOT__rx__DOT__cursor = VL_RAND_RESET_I(4);
    vlSelf->uart_top__DOT__rx__DOT__buffer = VL_RAND_RESET_I(8);
    vlSelf->uart_top__DOT__rx__DOT__sum = VL_RAND_RESET_I(32);
}
