// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vbasic_lockstep2.h for the primary calling header

#include "verilated.h"

#include "Vbasic_lockstep2__Syms.h"
#include "Vbasic_lockstep2___024root.h"

void Vbasic_lockstep2___024root___ctor_var_reset(Vbasic_lockstep2___024root* vlSelf);

Vbasic_lockstep2___024root::Vbasic_lockstep2___024root(const char* _vcname__)
    : VerilatedModule(_vcname__)
 {
    // Reset structure values
    Vbasic_lockstep2___024root___ctor_var_reset(this);
}

void Vbasic_lockstep2___024root::__Vconfigure(Vbasic_lockstep2__Syms* _vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->vlSymsp = _vlSymsp;
}

Vbasic_lockstep2___024root::~Vbasic_lockstep2___024root() {
}
