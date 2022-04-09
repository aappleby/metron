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
    CData/*0:0*/ toplevel__DOT__data_memory_bus_write_enable;
    CData/*3:0*/ toplevel__DOT__data_memory_bus_byte_enable;
    CData/*0:0*/ toplevel__DOT__core__DOT__datapath_regfile_write_enable;
    CData/*0:0*/ toplevel__DOT__core__DOT__datapath_pc_write_enable;
    CData/*0:0*/ __Vclklast__TOP__clock;
    VL_OUT(o_bus_read_data,31,0);
    VL_OUT(o_bus_address,31,0);
    VL_OUT(o_bus_write_data,31,0);
    VL_OUT(o_inst,31,0);
    VL_OUT(o_pc,31,0);
    IData/*31:0*/ toplevel__DOT__core___05Finst;
    IData/*31:0*/ toplevel__DOT__data_memory_bus_address;
    IData/*31:0*/ toplevel__DOT__data_memory_bus_write_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__program_counter_next;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__regs_rd_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value;
    VlUnpacked<IData/*31:0*/, 32> toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs;
    VlUnpacked<IData/*31:0*/, 16384> toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem;
    VlUnpacked<IData/*31:0*/, 32768> toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem;
    std::string toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__unnamedblk1__DOT__s;
    std::string toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__unnamedblk1__DOT__s;

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
