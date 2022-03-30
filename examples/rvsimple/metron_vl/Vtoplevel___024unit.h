// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtoplevel.h for the primary calling header

#ifndef VERILATED_VTOPLEVEL___024UNIT_H_
#define VERILATED_VTOPLEVEL___024UNIT_H_  // guard

#include "verilated.h"

class Vtoplevel__Syms;
VL_MODULE(Vtoplevel___024unit) {
  public:

    // INTERNAL VARIABLES
    Vtoplevel__Syms* vlSymsp;  // Symbol table

    // CONSTRUCTORS
    Vtoplevel___024unit(const char* name);
    ~Vtoplevel___024unit();
    VL_UNCOPYABLE(Vtoplevel___024unit);

    // INTERNAL METHODS
    void __Vconfigure(Vtoplevel__Syms* symsp, bool first);
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);


#endif  // guard
