// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vbasic_lockstep2.h for the primary calling header

#ifndef VERILATED_VBASIC_LOCKSTEP2___024ROOT_H_
#define VERILATED_VBASIC_LOCKSTEP2___024ROOT_H_  // guard

#include "verilated.h"

class Vbasic_lockstep2__Syms;
VL_MODULE(Vbasic_lockstep2___024root) {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clock,0,0);
    VL_OUT8(done,0,0);
    CData/*0:0*/ __Vclklast__TOP__clock;
    VL_OUT(result,31,0);
    IData/*31:0*/ Module__DOT__counter;

    // INTERNAL VARIABLES
    Vbasic_lockstep2__Syms* vlSymsp;  // Symbol table

    // CONSTRUCTORS
    Vbasic_lockstep2___024root(const char* name);
    ~Vbasic_lockstep2___024root();
    VL_UNCOPYABLE(Vbasic_lockstep2___024root);

    // INTERNAL METHODS
    void __Vconfigure(Vbasic_lockstep2__Syms* symsp, bool first);
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);


#endif  // guard
