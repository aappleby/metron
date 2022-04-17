// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vbasic_lockstep3.h for the primary calling header

#include "verilated.h"

#include "Vbasic_lockstep3___024root.h"

VL_INLINE_OPT void Vbasic_lockstep3___024root___sequent__TOP__0(Vbasic_lockstep3___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep3__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep3___024root___sequent__TOP__0\n"); );
    // Body
    vlSelf->Module__DOT__counter = ((IData)(1U) + vlSelf->Module__DOT__counter);
    vlSelf->done = (7U <= vlSelf->Module__DOT__counter);
    vlSelf->result = vlSelf->Module__DOT__counter;
}

void Vbasic_lockstep3___024root___eval(Vbasic_lockstep3___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep3__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep3___024root___eval\n"); );
    // Body
    if (((IData)(vlSelf->clock) & (~ (IData)(vlSelf->__Vclklast__TOP__clock)))) {
        Vbasic_lockstep3___024root___sequent__TOP__0(vlSelf);
    }
    // Final
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
}

#ifdef VL_DEBUG
void Vbasic_lockstep3___024root___eval_debug_assertions(Vbasic_lockstep3___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep3__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep3___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clock & 0xfeU))) {
        Verilated::overWidthError("clock");}
}
#endif  // VL_DEBUG
