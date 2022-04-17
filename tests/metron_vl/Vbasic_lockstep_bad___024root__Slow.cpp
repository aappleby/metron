// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vbasic_lockstep_bad.h for the primary calling header

#include "verilated.h"

#include "Vbasic_lockstep_bad__Syms.h"
#include "Vbasic_lockstep_bad___024root.h"

void Vbasic_lockstep_bad___024root___ctor_var_reset(Vbasic_lockstep_bad___024root* vlSelf);

Vbasic_lockstep_bad___024root::Vbasic_lockstep_bad___024root(const char* _vcname__)
    : VerilatedModule(_vcname__)
 {
    // Reset structure values
    Vbasic_lockstep_bad___024root___ctor_var_reset(this);
}

void Vbasic_lockstep_bad___024root::__Vconfigure(Vbasic_lockstep_bad__Syms* _vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->vlSymsp = _vlSymsp;
}

Vbasic_lockstep_bad___024root::~Vbasic_lockstep_bad___024root() {
}
