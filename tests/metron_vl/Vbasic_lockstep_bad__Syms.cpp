// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vbasic_lockstep_bad__Syms.h"
#include "Vbasic_lockstep_bad.h"
#include "Vbasic_lockstep_bad___024root.h"

// FUNCTIONS
Vbasic_lockstep_bad__Syms::~Vbasic_lockstep_bad__Syms()
{
}

Vbasic_lockstep_bad__Syms::Vbasic_lockstep_bad__Syms(VerilatedContext* contextp, const char* namep,Vbasic_lockstep_bad* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP(namep)
{
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(this, true);
}
