// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VUART_TOP__SYMS_H_
#define VERILATED_VUART_TOP__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vuart_top.h"

// INCLUDE MODULE CLASSES
#include "Vuart_top___024root.h"

// SYMS CLASS (contains all model state)
class Vuart_top__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vuart_top* const __Vm_modelp;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vuart_top___024root            TOP;

    // CONSTRUCTORS
    Vuart_top__Syms(VerilatedContext* contextp, const char* namep, Vuart_top* modelp);
    ~Vuart_top__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
