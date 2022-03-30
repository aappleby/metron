// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel__Syms.h"
#include "Vtoplevel___024unit.h"

void Vtoplevel___024unit___ctor_var_reset(Vtoplevel___024unit* vlSelf);

Vtoplevel___024unit::Vtoplevel___024unit(const char* _vcname__)
    : VerilatedModule(_vcname__)
 {
    // Reset structure values
    Vtoplevel___024unit___ctor_var_reset(this);
}

void Vtoplevel___024unit::__Vconfigure(Vtoplevel__Syms* _vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->vlSymsp = _vlSymsp;
}

Vtoplevel___024unit::~Vtoplevel___024unit() {
}
