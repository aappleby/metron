// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vuart_top.h"
#include "Vuart_top__Syms.h"

//============================================================
// Constructors

Vuart_top::Vuart_top(VerilatedContext* _vcontextp__, const char* _vcname__)
    : vlSymsp{new Vuart_top__Syms(_vcontextp__, _vcname__, this)}
    , clock{vlSymsp->TOP.clock}
    , tock_i_rstn{vlSymsp->TOP.tock_i_rstn}
    , tock_serial{vlSymsp->TOP.tock_serial}
    , tock_data{vlSymsp->TOP.tock_data}
    , tock_valid{vlSymsp->TOP.tock_valid}
    , tock_done{vlSymsp->TOP.tock_done}
    , tock_sum{vlSymsp->TOP.tock_sum}
    , rootp{&(vlSymsp->TOP)}
{
}

Vuart_top::Vuart_top(const char* _vcname__)
    : Vuart_top(nullptr, _vcname__)
{
}

//============================================================
// Destructor

Vuart_top::~Vuart_top() {
    delete vlSymsp;
}

//============================================================
// Evaluation loop

void Vuart_top___024root___eval_initial(Vuart_top___024root* vlSelf);
void Vuart_top___024root___eval_settle(Vuart_top___024root* vlSelf);
void Vuart_top___024root___eval(Vuart_top___024root* vlSelf);
#ifdef VL_DEBUG
void Vuart_top___024root___eval_debug_assertions(Vuart_top___024root* vlSelf);
#endif  // VL_DEBUG
void Vuart_top___024root___final(Vuart_top___024root* vlSelf);

static void _eval_initial_loop(Vuart_top__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    Vuart_top___024root___eval_initial(&(vlSymsp->TOP));
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial loop\n"););
        Vuart_top___024root___eval_settle(&(vlSymsp->TOP));
        Vuart_top___024root___eval(&(vlSymsp->TOP));
    } while (0);
}

void Vuart_top::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vuart_top::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vuart_top___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        Vuart_top___024root___eval(&(vlSymsp->TOP));
    } while (0);
    // Evaluate cleanup
}

//============================================================
// Utilities

VerilatedContext* Vuart_top::contextp() const {
    return vlSymsp->_vm_contextp__;
}

const char* Vuart_top::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

VL_ATTR_COLD void Vuart_top::final() {
    Vuart_top___024root___final(&(vlSymsp->TOP));
}
