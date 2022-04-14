// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vuart_top.h for the primary calling header

#include "verilated.h"

#include "Vuart_top___024root.h"

VL_INLINE_OPT void Vuart_top___024root___sequent__TOP__0(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___sequent__TOP__0\n"); );
    // Init
    CData/*7:0*/ uart_top__DOT__unnamedblk1__DOT__hello_data;
    CData/*0:0*/ uart_top__DOT__unnamedblk1__DOT__hello_req;
    CData/*1:0*/ __Vdly__uart_top__DOT__hello__DOT__state;
    CData/*1:0*/ __Vdly__uart_top__DOT__tx__DOT__cycle;
    CData/*4:0*/ __Vdly__uart_top__DOT__tx__DOT__cursor;
    SData/*8:0*/ __Vdly__uart_top__DOT__tx__DOT__buffer;
    // Body
    __Vdly__uart_top__DOT__tx__DOT__buffer = vlSelf->uart_top__DOT__tx__DOT__buffer;
    __Vdly__uart_top__DOT__hello__DOT__state = vlSelf->uart_top__DOT__hello__DOT__state;
    __Vdly__uart_top__DOT__tx__DOT__cursor = vlSelf->uart_top__DOT__tx__DOT__cursor;
    __Vdly__uart_top__DOT__tx__DOT__cycle = vlSelf->uart_top__DOT__tx__DOT__cycle;
    if (vlSelf->tock_i_rstn) {
        if ((0U != (IData)(vlSelf->uart_top__DOT__rx__DOT__cycle))) {
            vlSelf->uart_top__DOT__rx__DOT__cycle = 
                (3U & ((IData)(vlSelf->uart_top__DOT__rx__DOT__cycle) 
                       - (IData)(1U)));
        } else if ((0U != (IData)(vlSelf->uart_top__DOT__rx__DOT__cursor))) {
            vlSelf->uart_top__DOT__rx__DOT__unnamedblk1__DOT__temp 
                = (0xffU & ((0x80U & ((IData)(vlSelf->uart_top__DOT__tx__DOT__buffer) 
                                      << 7U)) | ((IData)(vlSelf->uart_top__DOT__rx__DOT__buffer) 
                                                 >> 1U)));
            if ((1U == ((IData)(vlSelf->uart_top__DOT__rx__DOT__cursor) 
                        - (IData)(1U)))) {
                vlSelf->uart_top__DOT__rx__DOT__sum 
                    = (vlSelf->uart_top__DOT__rx__DOT__sum 
                       + (IData)(vlSelf->uart_top__DOT__rx__DOT__unnamedblk1__DOT__temp));
            }
            vlSelf->uart_top__DOT__rx__DOT__cycle = 2U;
            vlSelf->uart_top__DOT__rx__DOT__cursor 
                = (0xfU & ((IData)(vlSelf->uart_top__DOT__rx__DOT__cursor) 
                           - (IData)(1U)));
            vlSelf->uart_top__DOT__rx__DOT__buffer 
                = vlSelf->uart_top__DOT__rx__DOT__unnamedblk1__DOT__temp;
        } else if ((1U & (~ (IData)(vlSelf->uart_top__DOT__tx__DOT__buffer)))) {
            vlSelf->uart_top__DOT__rx__DOT__cycle = 2U;
            vlSelf->uart_top__DOT__rx__DOT__cursor = 9U;
        }
        vlSelf->uart_top__DOT__hello__DOT__data = vlSelf->uart_top__DOT__hello__DOT__memory
            [vlSelf->uart_top__DOT__hello__DOT__cursor];
        if (((0U == (IData)(vlSelf->uart_top__DOT__hello__DOT__state)) 
             & (IData)(vlSelf->uart_top__DOT__tx_tock_idle))) {
            __Vdly__uart_top__DOT__hello__DOT__state = 1U;
        } else if (((1U == (IData)(vlSelf->uart_top__DOT__hello__DOT__state)) 
                    & (((7U == (IData)(vlSelf->uart_top__DOT__tx__DOT__cursor)) 
                        & (0U == (IData)(vlSelf->uart_top__DOT__tx__DOT__cycle))) 
                       | (7U > (IData)(vlSelf->uart_top__DOT__tx__DOT__cursor))))) {
            if ((0x1ffU == (IData)(vlSelf->uart_top__DOT__hello__DOT__cursor))) {
                __Vdly__uart_top__DOT__hello__DOT__state = 2U;
            } else {
                vlSelf->uart_top__DOT__hello__DOT__cursor 
                    = (0x1ffU & ((IData)(1U) + (IData)(vlSelf->uart_top__DOT__hello__DOT__cursor)));
            }
        } else if ((2U == (IData)(vlSelf->uart_top__DOT__hello__DOT__state))) {
            vlSelf->uart_top__DOT__hello__DOT__cursor = 0U;
        }
        vlSelf->uart_top__DOT__tx__DOT__unnamedblk1__DOT__cycle_max = 2U;
        vlSelf->uart_top__DOT__tx__DOT__unnamedblk1__DOT__cursor_max = 0x10U;
        if ((((7U >= (IData)(vlSelf->uart_top__DOT__tx__DOT__cursor)) 
              & (0U == (IData)(vlSelf->uart_top__DOT__tx__DOT__cycle))) 
             & (IData)(vlSelf->uart_top__DOT__tx_tock_i_req))) {
            __Vdly__uart_top__DOT__tx__DOT__cycle = vlSelf->uart_top__DOT__tx__DOT__unnamedblk1__DOT__cycle_max;
            __Vdly__uart_top__DOT__tx__DOT__cursor 
                = vlSelf->uart_top__DOT__tx__DOT__unnamedblk1__DOT__cursor_max;
            __Vdly__uart_top__DOT__tx__DOT__buffer 
                = (0x1ffU & ((IData)(vlSelf->uart_top__DOT__tx_tock_i_data) 
                             << 1U));
        } else if ((0U != (IData)(vlSelf->uart_top__DOT__tx__DOT__cycle))) {
            __Vdly__uart_top__DOT__tx__DOT__cycle = 
                (3U & ((IData)(vlSelf->uart_top__DOT__tx__DOT__cycle) 
                       - (IData)(1U)));
            __Vdly__uart_top__DOT__tx__DOT__cursor 
                = vlSelf->uart_top__DOT__tx__DOT__cursor;
            __Vdly__uart_top__DOT__tx__DOT__buffer 
                = vlSelf->uart_top__DOT__tx__DOT__buffer;
        } else if ((0U != (IData)(vlSelf->uart_top__DOT__tx__DOT__cursor))) {
            __Vdly__uart_top__DOT__tx__DOT__cursor 
                = (0x1fU & ((IData)(vlSelf->uart_top__DOT__tx__DOT__cursor) 
                            - (IData)(1U)));
            __Vdly__uart_top__DOT__tx__DOT__buffer 
                = (0x1ffU & (0x100U | ((IData)(vlSelf->uart_top__DOT__tx__DOT__buffer) 
                                       >> 1U)));
            __Vdly__uart_top__DOT__tx__DOT__cycle = vlSelf->uart_top__DOT__tx__DOT__unnamedblk1__DOT__cycle_max;
        }
    } else {
        vlSelf->uart_top__DOT__rx__DOT__cycle = 0U;
        vlSelf->uart_top__DOT__rx__DOT__cursor = 0U;
        vlSelf->uart_top__DOT__rx__DOT__buffer = 0U;
        vlSelf->uart_top__DOT__rx__DOT__sum = 0U;
        __Vdly__uart_top__DOT__hello__DOT__state = 0U;
        vlSelf->uart_top__DOT__hello__DOT__cursor = 0U;
        __Vdly__uart_top__DOT__tx__DOT__cycle = 0U;
        __Vdly__uart_top__DOT__tx__DOT__cursor = 0U;
        __Vdly__uart_top__DOT__tx__DOT__buffer = 0x1ffU;
    }
    vlSelf->uart_top__DOT__hello__DOT__state = __Vdly__uart_top__DOT__hello__DOT__state;
    vlSelf->uart_top__DOT__tx__DOT__buffer = __Vdly__uart_top__DOT__tx__DOT__buffer;
    vlSelf->uart_top__DOT__tx__DOT__cycle = __Vdly__uart_top__DOT__tx__DOT__cycle;
    vlSelf->uart_top__DOT__tx__DOT__cursor = __Vdly__uart_top__DOT__tx__DOT__cursor;
    vlSelf->tock_sum = vlSelf->uart_top__DOT__rx__DOT__sum;
    vlSelf->tock_valid = (1U == (IData)(vlSelf->uart_top__DOT__rx__DOT__cursor));
    vlSelf->tock_data = vlSelf->uart_top__DOT__rx__DOT__buffer;
    uart_top__DOT__unnamedblk1__DOT__hello_data = vlSelf->uart_top__DOT__hello__DOT__data;
    vlSelf->uart_top__DOT__tx_tock_i_data = uart_top__DOT__unnamedblk1__DOT__hello_data;
    uart_top__DOT__unnamedblk1__DOT__hello_req = (1U 
                                                  == (IData)(vlSelf->uart_top__DOT__hello__DOT__state));
    vlSelf->uart_top__DOT__tx_tock_i_req = uart_top__DOT__unnamedblk1__DOT__hello_req;
    vlSelf->tock_serial = (1U & (IData)(vlSelf->uart_top__DOT__tx__DOT__buffer));
    vlSelf->uart_top__DOT__tx_tock_idle = ((0U == (IData)(vlSelf->uart_top__DOT__tx__DOT__cursor)) 
                                           & (0U == (IData)(vlSelf->uart_top__DOT__tx__DOT__cycle)));
    vlSelf->tock_done = ((2U == (IData)(vlSelf->uart_top__DOT__hello__DOT__state)) 
                         & (IData)(vlSelf->uart_top__DOT__tx_tock_idle));
}

void Vuart_top___024root___eval(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___eval\n"); );
    // Body
    if (((IData)(vlSelf->clock) & (~ (IData)(vlSelf->__Vclklast__TOP__clock)))) {
        Vuart_top___024root___sequent__TOP__0(vlSelf);
    }
    // Final
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
}

#ifdef VL_DEBUG
void Vuart_top___024root___eval_debug_assertions(Vuart_top___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vuart_top__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vuart_top___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clock & 0xfeU))) {
        Verilated::overWidthError("clock");}
    if (VL_UNLIKELY((vlSelf->tock_i_rstn & 0xfeU))) {
        Verilated::overWidthError("tock_i_rstn");}
}
#endif  // VL_DEBUG
