// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vuart_top.h for the primary calling header

#ifndef VERILATED_VUART_TOP___024ROOT_H_
#define VERILATED_VUART_TOP___024ROOT_H_  // guard

#include "verilated.h"

class Vuart_top__Syms;
VL_MODULE(Vuart_top___024root) {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clock,0,0);
    VL_IN8(i_rstn,0,0);
    VL_OUT8(o_data,7,0);
    VL_OUT8(o_valid,0,0);
    VL_OUT8(o_done,0,0);
    CData/*7:0*/ uart_top__DOT__tx_i_data;
    CData/*0:0*/ uart_top__DOT__tx_i_req;
    CData/*0:0*/ uart_top__DOT__tx_o_idle;
    CData/*1:0*/ uart_top__DOT__hello__DOT__state;
    CData/*7:0*/ uart_top__DOT__hello__DOT__data;
    CData/*1:0*/ uart_top__DOT__tx__DOT__cycle;
    CData/*4:0*/ uart_top__DOT__tx__DOT__cursor;
    CData/*1:0*/ uart_top__DOT__rx__DOT__cycle;
    CData/*3:0*/ uart_top__DOT__rx__DOT__cursor;
    CData/*7:0*/ uart_top__DOT__rx__DOT__buffer;
    CData/*0:0*/ __Vclklast__TOP__clock;
    SData/*8:0*/ uart_top__DOT__hello__DOT__cursor;
    SData/*8:0*/ uart_top__DOT__tx__DOT__buffer;
    VL_OUT(o_sum,31,0);
    IData/*31:0*/ uart_top__DOT__rx__DOT__sum;
    VlUnpacked<CData/*7:0*/, 512> uart_top__DOT__hello__DOT__memory;

    // INTERNAL VARIABLES
    Vuart_top__Syms* vlSymsp;  // Symbol table

    // CONSTRUCTORS
    Vuart_top___024root(const char* name);
    ~Vuart_top___024root();
    VL_UNCOPYABLE(Vuart_top___024root);

    // INTERNAL METHODS
    void __Vconfigure(Vuart_top__Syms* symsp, bool first);
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);


#endif  // guard
