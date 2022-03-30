// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTOPLEVEL__SYMS_H_
#define VERILATED_VTOPLEVEL__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtoplevel.h"

// INCLUDE MODULE CLASSES
#include "Vtoplevel___024root.h"
#include "Vtoplevel___024unit.h"

// SYMS CLASS (contains all model state)
class Vtoplevel__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtoplevel* const __Vm_modelp;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtoplevel___024root            TOP;

    // CONSTRUCTORS
    Vtoplevel__Syms(VerilatedContext* contextp, const char* namep, Vtoplevel* modelp);
    ~Vtoplevel__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
