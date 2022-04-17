// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vbasic_lockstep.h for the primary calling header

#include "verilated.h"

#include "Vbasic_lockstep__Syms.h"
#include "Vbasic_lockstep___024root.h"

void Vbasic_lockstep___024root___ctor_var_reset(Vbasic_lockstep___024root* vlSelf);

Vbasic_lockstep___024root::Vbasic_lockstep___024root(const char* _vcname__)
    : VerilatedModule(_vcname__)
 {
    // Reset structure values
    Vbasic_lockstep___024root___ctor_var_reset(this);
}

void Vbasic_lockstep___024root::__Vconfigure(Vbasic_lockstep__Syms* _vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->vlSymsp = _vlSymsp;
}

Vbasic_lockstep___024root::~Vbasic_lockstep___024root() {
}
