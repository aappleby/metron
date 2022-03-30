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
    VL_OUT8(bus_byte_enable,3,0);
    VL_OUT8(bus_read_enable,0,0);
    VL_OUT8(bus_write_enable,0,0);
    CData/*0:0*/ toplevel__DOT__riscv_core__DOT__pc_write_enable;
    CData/*0:0*/ toplevel__DOT__riscv_core__DOT__regfile_write_enable;
    CData/*2:0*/ toplevel__DOT__riscv_core__DOT__reg_writeback_select;
    CData/*0:0*/ toplevel__DOT__riscv_core__DOT__singlecycle_ctlpath__DOT__take_branch;
    CData/*0:0*/ toplevel__DOT__data_memory_bus__DOT__is_data_memory;
    CData/*0:0*/ __Vclklast__TOP__clock;
    CData/*0:0*/ __Vclklast__TOP__reset;
    VL_OUT(bus_read_data,31,0);
    VL_OUT(bus_address,31,0);
    VL_OUT(bus_write_data,31,0);
    VL_OUT(inst,31,0);
    VL_OUT(pc,31,0);
    VlUnpacked<IData/*31:0*/, 4> toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_next_pc_select__DOT__multiplexer__DOT__input_array;
    VlUnpacked<IData/*31:0*/, 2> toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_a__DOT__multiplexer__DOT__input_array;
    VlUnpacked<IData/*31:0*/, 2> toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_operand_b__DOT__multiplexer__DOT__input_array;
    VlUnpacked<IData/*31:0*/, 8> toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__mux_reg_writeback__DOT__multiplexer__DOT__input_array;
    VlUnpacked<IData/*31:0*/, 32> toplevel__DOT__riscv_core__DOT__singlecycle_datapath__DOT__regfile__DOT__register;
    VlUnpacked<IData/*31:0*/, 16384> toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem;
    VlUnpacked<IData/*31:0*/, 32768> toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem;

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
