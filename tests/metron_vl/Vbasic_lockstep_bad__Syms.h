// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VBASIC_LOCKSTEP_BAD__SYMS_H_
#define VERILATED_VBASIC_LOCKSTEP_BAD__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vbasic_lockstep_bad.h"

// INCLUDE MODULE CLASSES
#include "Vbasic_lockstep_bad___024root.h"

// SYMS CLASS (contains all model state)
class Vbasic_lockstep_bad__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vbasic_lockstep_bad* const __Vm_modelp;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vbasic_lockstep_bad___024root  TOP;

    // CONSTRUCTORS
    Vbasic_lockstep_bad__Syms(VerilatedContext* contextp, const char* namep, Vbasic_lockstep_bad* modelp);
    ~Vbasic_lockstep_bad__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
