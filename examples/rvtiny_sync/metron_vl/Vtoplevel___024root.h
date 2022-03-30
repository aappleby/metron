// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtoplevel.h for the primary calling header

#ifndef VERILATED_VTOPLEVEL___024ROOT_H_
#define VERILATED_VTOPLEVEL___024ROOT_H_  // guard

#include "verilated.h"

class Vtoplevel__Syms;
VL_MODULE(Vtoplevel___024root) {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clock,0,0);
    VL_IN8(reset,0,0);
    VL_OUT8(o_bus_byte_enable,3,0);
    VL_OUT8(o_bus_read_enable,0,0);
    VL_OUT8(o_bus_write_enable,0,0);
    CData/*6:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__op;
    CData/*4:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__rd;
    CData/*2:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__f3;
    CData/*4:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__r1;
    CData/*4:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__r2;
    CData/*6:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__f7;
    CData/*0:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk5__DOT__take_branch;
    CData/*0:0*/ __Vclklast__TOP__clock;
    SData/*14:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk4__DOT__phys_addr;
    VL_OUT(o_bus_read_data,31,0);
    VL_OUT(o_bus_address,31,0);
    VL_OUT(o_bus_write_data,31,0);
    VL_OUT(o_inst,31,0);
    VL_OUT(o_pc,31,0);
    IData/*31:0*/ toplevel__DOT__pc;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__inst;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk2__DOT__op_a;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk2__DOT__op_b;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk2__DOT__alu_result;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk3__DOT__imm;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk3__DOT__addr;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk3__DOT__data;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk4__DOT__imm;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk4__DOT__addr;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk4__DOT__data;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk4__DOT__mask;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk5__DOT__op_a;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk5__DOT__op_b;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk5__DOT__unnamedblk6__DOT__imm;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk7__DOT__imm;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk8__DOT__imm;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk9__DOT__imm;
    IData/*31:0*/ toplevel__DOT__tick__DOT__unnamedblk1__DOT__unnamedblk10__DOT__imm;
    VlUnpacked<IData/*31:0*/, 32768> toplevel__DOT__text_mem;
    VlUnpacked<IData/*31:0*/, 32768> toplevel__DOT__data_mem;
    VlUnpacked<IData/*31:0*/, 32> toplevel__DOT__regs;
    std::string toplevel__DOT__init__DOT__s;

    // INTERNAL VARIABLES
    Vtoplevel__Syms* vlSymsp;  // Symbol table

    // CONSTRUCTORS
    Vtoplevel___024root(const char* name);
    ~Vtoplevel___024root();
    VL_UNCOPYABLE(Vtoplevel___024root);

    // INTERNAL METHODS
    void __Vconfigure(Vtoplevel__Syms* symsp, bool first);
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);


#endif  // guard
