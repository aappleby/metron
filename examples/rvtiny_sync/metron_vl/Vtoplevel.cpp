// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtoplevel.h"
#include "Vtoplevel__Syms.h"

//============================================================
// Constructors

Vtoplevel::Vtoplevel(VerilatedContext* _vcontextp__, const char* _vcname__)
    : vlSymsp{new Vtoplevel__Syms(_vcontextp__, _vcname__, this)}
    , clock{vlSymsp->TOP.clock}
    , o_bus_byte_enable{vlSymsp->TOP.o_bus_byte_enable}
    , o_bus_read_enable{vlSymsp->TOP.o_bus_read_enable}
    , o_bus_write_enable{vlSymsp->TOP.o_bus_write_enable}
    , tock_reset{vlSymsp->TOP.tock_reset}
    , o_bus_read_data{vlSymsp->TOP.o_bus_read_data}
    , o_bus_address{vlSymsp->TOP.o_bus_address}
    , o_bus_write_data{vlSymsp->TOP.o_bus_write_data}
    , o_inst{vlSymsp->TOP.o_inst}
    , o_pc{vlSymsp->TOP.o_pc}
    , rootp{&(vlSymsp->TOP)}
{
}

Vtoplevel::Vtoplevel(const char* _vcname__)
    : Vtoplevel(nullptr, _vcname__)
{
}

//============================================================
// Destructor

Vtoplevel::~Vtoplevel() {
    delete vlSymsp;
}

//============================================================
// Evaluation loop

void Vtoplevel___024root___eval_initial(Vtoplevel___024root* vlSelf);
void Vtoplevel___024root___eval_settle(Vtoplevel___024root* vlSelf);
void Vtoplevel___024root___eval(Vtoplevel___024root* vlSelf);
#ifdef VL_DEBUG
void Vtoplevel___024root___eval_debug_assertions(Vtoplevel___024root* vlSelf);
#endif  // VL_DEBUG
void Vtoplevel___024root___final(Vtoplevel___024root* vlSelf);

static void _eval_initial_loop(Vtoplevel__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    Vtoplevel___024root___eval_initial(&(vlSymsp->TOP));
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial loop\n"););
        Vtoplevel___024root___eval_settle(&(vlSymsp->TOP));
        Vtoplevel___024root___eval(&(vlSymsp->TOP));
    } while (0);
}

void Vtoplevel::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtoplevel::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtoplevel___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        Vtoplevel___024root___eval(&(vlSymsp->TOP));
    } while (0);
    // Evaluate cleanup
}

//============================================================
// Utilities

VerilatedContext* Vtoplevel::contextp() const {
    return vlSymsp->_vm_contextp__;
}

const char* Vtoplevel::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

VL_ATTR_COLD void Vtoplevel::final() {
    Vtoplevel___024root___final(&(vlSymsp->TOP));
}
