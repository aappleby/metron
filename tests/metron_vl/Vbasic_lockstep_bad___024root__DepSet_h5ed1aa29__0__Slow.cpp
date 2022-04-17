// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vbasic_lockstep_bad.h for the primary calling header

#include "verilated.h"

#include "Vbasic_lockstep_bad___024root.h"

VL_ATTR_COLD void Vbasic_lockstep_bad___024root___initial__TOP__0(Vbasic_lockstep_bad___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep_bad__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep_bad___024root___initial__TOP__0\n"); );
    // Body
    vlSelf->Module__DOT__counter = 0U;
}

VL_ATTR_COLD void Vbasic_lockstep_bad___024root___settle__TOP__0(Vbasic_lockstep_bad___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep_bad__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep_bad___024root___settle__TOP__0\n"); );
    // Init
    IData/*31:0*/ Module__DOT__unnamedblk1__DOT__c;
    // Body
    vlSelf->done = (7U <= vlSelf->Module__DOT__counter);
    Module__DOT__unnamedblk1__DOT__c = vlSelf->Module__DOT__counter;
    Module__DOT__unnamedblk1__DOT__c = ((IData)(1U) 
                                        + Module__DOT__unnamedblk1__DOT__c);
    vlSelf->result = Module__DOT__unnamedblk1__DOT__c;
}

VL_ATTR_COLD void Vbasic_lockstep_bad___024root___eval_initial(Vbasic_lockstep_bad___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep_bad__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep_bad___024root___eval_initial\n"); );
    // Body
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
    Vbasic_lockstep_bad___024root___initial__TOP__0(vlSelf);
}

VL_ATTR_COLD void Vbasic_lockstep_bad___024root___eval_settle(Vbasic_lockstep_bad___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep_bad__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep_bad___024root___eval_settle\n"); );
    // Body
    Vbasic_lockstep_bad___024root___settle__TOP__0(vlSelf);
}

VL_ATTR_COLD void Vbasic_lockstep_bad___024root___final(Vbasic_lockstep_bad___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep_bad__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep_bad___024root___final\n"); );
}

VL_ATTR_COLD void Vbasic_lockstep_bad___024root___ctor_var_reset(Vbasic_lockstep_bad___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vbasic_lockstep_bad__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vbasic_lockstep_bad___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clock = VL_RAND_RESET_I(1);
    vlSelf->done = VL_RAND_RESET_I(1);
    vlSelf->result = VL_RAND_RESET_I(32);
    vlSelf->Module__DOT__counter = VL_RAND_RESET_I(32);
}
