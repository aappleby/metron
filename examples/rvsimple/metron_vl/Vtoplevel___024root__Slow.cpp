// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel__Syms.h"
#include "Vtoplevel___024root.h"

void Vtoplevel___024root___ctor_var_reset(Vtoplevel___024root* vlSelf);

Vtoplevel___024root::Vtoplevel___024root(const char* _vcname__)
    : VerilatedModule(_vcname__)
 {
    // Reset structure values
    Vtoplevel___024root___ctor_var_reset(this);
}

void Vtoplevel___024root::__Vconfigure(Vtoplevel__Syms* _vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->vlSymsp = _vlSymsp;
}

Vtoplevel___024root::~Vtoplevel___024root() {
}
