// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vbasic_lockstep2__Syms.h"
#include "Vbasic_lockstep2.h"
#include "Vbasic_lockstep2___024root.h"

// FUNCTIONS
Vbasic_lockstep2__Syms::~Vbasic_lockstep2__Syms()
{
}

Vbasic_lockstep2__Syms::Vbasic_lockstep2__Syms(VerilatedContext* contextp, const char* namep,Vbasic_lockstep2* modelp)
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
