// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vuart_top.h for the primary calling header

#include "verilated.h"

#include "Vuart_top__Syms.h"
#include "Vuart_top___024root.h"

void Vuart_top___024root___ctor_var_reset(Vuart_top___024root* vlSelf);

Vuart_top___024root::Vuart_top___024root(const char* _vcname__)
    : VerilatedModule(_vcname__)
 {
    // Reset structure values
    Vuart_top___024root___ctor_var_reset(this);
}

void Vuart_top___024root::__Vconfigure(Vuart_top__Syms* _vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->vlSymsp = _vlSymsp;
}

Vuart_top___024root::~Vuart_top___024root() {
}
