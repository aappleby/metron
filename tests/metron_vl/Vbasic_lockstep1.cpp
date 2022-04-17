// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vbasic_lockstep1.h"
#include "Vbasic_lockstep1__Syms.h"

//============================================================
// Constructors

Vbasic_lockstep1::Vbasic_lockstep1(VerilatedContext* _vcontextp__, const char* _vcname__)
    : vlSymsp{new Vbasic_lockstep1__Syms(_vcontextp__, _vcname__, this)}
    , clock{vlSymsp->TOP.clock}
    , done{vlSymsp->TOP.done}
    , result{vlSymsp->TOP.result}
    , rootp{&(vlSymsp->TOP)}
{
}

Vbasic_lockstep1::Vbasic_lockstep1(const char* _vcname__)
    : Vbasic_lockstep1(nullptr, _vcname__)
{
}

//============================================================
// Destructor

Vbasic_lockstep1::~Vbasic_lockstep1() {
    delete vlSymsp;
}

//============================================================
// Evaluation loop

void Vbasic_lockstep1___024root___eval_initial(Vbasic_lockstep1___024root* vlSelf);
void Vbasic_lockstep1___024root___eval_settle(Vbasic_lockstep1___024root* vlSelf);
void Vbasic_lockstep1___024root___eval(Vbasic_lockstep1___024root* vlSelf);
#ifdef VL_DEBUG
void Vbasic_lockstep1___024root___eval_debug_assertions(Vbasic_lockstep1___024root* vlSelf);
#endif  // VL_DEBUG
void Vbasic_lockstep1___024root___final(Vbasic_lockstep1___024root* vlSelf);

static void _eval_initial_loop(Vbasic_lockstep1__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    Vbasic_lockstep1___024root___eval_initial(&(vlSymsp->TOP));
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial loop\n"););
        Vbasic_lockstep1___024root___eval_settle(&(vlSymsp->TOP));
        Vbasic_lockstep1___024root___eval(&(vlSymsp->TOP));
    } while (0);
}

void Vbasic_lockstep1::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vbasic_lockstep1::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vbasic_lockstep1___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        Vbasic_lockstep1___024root___eval(&(vlSymsp->TOP));
    } while (0);
    // Evaluate cleanup
}

//============================================================
// Utilities

VerilatedContext* Vbasic_lockstep1::contextp() const {
    return vlSymsp->_vm_contextp__;
}

const char* Vbasic_lockstep1::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

VL_ATTR_COLD void Vbasic_lockstep1::final() {
    Vbasic_lockstep1___024root___final(&(vlSymsp->TOP));
}
