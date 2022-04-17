// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VBASIC_LOCKSTEP3__SYMS_H_
#define VERILATED_VBASIC_LOCKSTEP3__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vbasic_lockstep3.h"

// INCLUDE MODULE CLASSES
#include "Vbasic_lockstep3___024root.h"

// SYMS CLASS (contains all model state)
class Vbasic_lockstep3__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vbasic_lockstep3* const __Vm_modelp;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vbasic_lockstep3___024root     TOP;

    // CONSTRUCTORS
    Vbasic_lockstep3__Syms(VerilatedContext* contextp, const char* namep, Vbasic_lockstep3* modelp);
    ~Vbasic_lockstep3__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
