// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VBASIC_LOCKSTEP2__SYMS_H_
#define VERILATED_VBASIC_LOCKSTEP2__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vbasic_lockstep2.h"

// INCLUDE MODULE CLASSES
#include "Vbasic_lockstep2___024root.h"

// SYMS CLASS (contains all model state)
class Vbasic_lockstep2__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vbasic_lockstep2* const __Vm_modelp;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vbasic_lockstep2___024root     TOP;

    // CONSTRUCTORS
    Vbasic_lockstep2__Syms(VerilatedContext* contextp, const char* namep, Vbasic_lockstep2* modelp);
    ~Vbasic_lockstep2__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
