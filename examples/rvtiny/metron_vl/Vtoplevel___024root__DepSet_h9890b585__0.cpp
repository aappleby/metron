// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel___024root.h"

VL_INLINE_OPT void Vtoplevel___024root___sequent__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___sequent__TOP__0\n"); );
    // Init
    IData/*31:0*/ __Vdly__pc;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__regs__v0;
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__regs__v1;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__regs__v1;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__regs__v1;
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__regs__v2;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__regs__v2;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__regs__v2;
    SData/*14:0*/ __Vdlyvdim0__toplevel__DOT__data_mem__v0;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__data_mem__v0;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__data_mem__v0;
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__regs__v3;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__regs__v3;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__regs__v3;
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__regs__v4;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__regs__v4;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__regs__v4;
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__regs__v5;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__regs__v5;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__regs__v5;
    CData/*4:0*/ __Vdlyvdim0__toplevel__DOT__regs__v6;
    IData/*31:0*/ __Vdlyvval__toplevel__DOT__regs__v6;
    CData/*0:0*/ __Vdlyvset__toplevel__DOT__regs__v6;
    // Body
    __Vdly__pc = vlSelf->pc;
    __Vdlyvset__toplevel__DOT__data_mem__v0 = 0U;
    __Vdlyvset__toplevel__DOT__regs__v0 = 0U;
    __Vdlyvset__toplevel__DOT__regs__v1 = 0U;
    __Vdlyvset__toplevel__DOT__regs__v2 = 0U;
    __Vdlyvset__toplevel__DOT__regs__v3 = 0U;
    __Vdlyvset__toplevel__DOT__regs__v4 = 0U;
    __Vdlyvset__toplevel__DOT__regs__v5 = 0U;
    __Vdlyvset__toplevel__DOT__regs__v6 = 0U;
    if (vlSelf->tock_reset) {
        vlSelf->inst = 0U;
        vlSelf->bus_byte_enable = 0U;
        vlSelf->bus_read_enable = 0U;
        vlSelf->bus_read_data = 0U;
        __Vdly__pc = 0U;
        __Vdlyvset__toplevel__DOT__regs__v0 = 1U;
        vlSelf->bus_address = 0U;
        vlSelf->bus_write_data = 0U;
        vlSelf->bus_write_enable = 0U;
        __Vdly__pc = 0U;
    } else {
        vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
            = vlSelf->toplevel__DOT__text_mem[(0x3fffU 
                                               & (vlSelf->pc 
                                                  >> 2U))];
        vlSelf->toplevel__DOT__unnamedblk2__DOT__op 
            = (0x7fU & vlSelf->toplevel__DOT__unnamedblk2__DOT__inst);
        vlSelf->toplevel__DOT__unnamedblk2__DOT__rd 
            = (0x1fU & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                        >> 7U));
        vlSelf->toplevel__DOT__unnamedblk2__DOT__f3 
            = (7U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                     >> 0xcU));
        vlSelf->toplevel__DOT__unnamedblk2__DOT__r1 
            = (0x1fU & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                        >> 0xfU));
        vlSelf->toplevel__DOT__unnamedblk2__DOT__r2 
            = (0x1fU & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                        >> 0x14U));
        vlSelf->toplevel__DOT__unnamedblk2__DOT__f7 
            = (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
               >> 0x19U);
        vlSelf->bus_address = 0U;
        vlSelf->bus_write_enable = 0U;
        vlSelf->bus_write_data = 0U;
        if (((0x33U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op)) 
             | (0x13U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op)))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                = vlSelf->toplevel__DOT__regs[vlSelf->toplevel__DOT__unnamedblk2__DOT__r1];
            __Vdly__pc = ((IData)(4U) + vlSelf->pc);
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b 
                = ((0x13U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))
                    ? (((- (IData)((vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                    >> 0x1fU))) << 0xbU) 
                       | (0x7ffU & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                    >> 0x14U))) : vlSelf->toplevel__DOT__regs
                   [vlSelf->toplevel__DOT__unnamedblk2__DOT__r2]);
            if (((((((((0U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3)) 
                       | (1U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) 
                      | (2U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) 
                     | (3U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) 
                    | (4U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) 
                   | (5U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) 
                  | (6U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) 
                 | (7U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3)))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__alu_result 
                    = ((0U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                        ? (((0x33U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op)) 
                            & ((IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f7) 
                               >> 5U)) ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                                          - vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b)
                            : (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                               + vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b))
                        : ((1U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                            ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                               << (0x1fU & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b))
                            : ((2U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                                ? VL_LTS_III(32, vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a, vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b)
                                : ((3U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                                    ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                                       < vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b)
                                    : ((4U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                                        ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                                           ^ vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b)
                                        : ((5U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                                            ? ((0x20U 
                                                & (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f7))
                                                ? VL_SHIFTRS_III(32,32,5, vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a, 
                                                                 (0x1fU 
                                                                  & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b))
                                                : (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                                                   >> 
                                                   (0x1fU 
                                                    & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b)))
                                            : ((6U 
                                                == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                                                ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                                                   | vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b)
                                                : (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_a 
                                                   & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__op_b))))))));
            }
            if ((0U != (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__rd))) {
                __Vdlyvval__toplevel__DOT__regs__v1 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk3__DOT__alu_result;
                __Vdlyvset__toplevel__DOT__regs__v1 = 1U;
                __Vdlyvdim0__toplevel__DOT__regs__v1 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__rd;
            }
        } else if ((3U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__imm 
                = (((- (IData)((vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                >> 0x1fU))) << 0xbU) 
                   | (0x7ffU & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                >> 0x14U)));
            __Vdly__pc = ((IData)(4U) + vlSelf->pc);
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__addr 
                = (vlSelf->toplevel__DOT__regs[vlSelf->toplevel__DOT__unnamedblk2__DOT__r1] 
                   + vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__imm);
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data 
                = ((0x1fU >= (0x18U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__addr 
                                       << 3U))) ? (
                                                   vlSelf->toplevel__DOT__data_mem
                                                   [
                                                   (0x7fffU 
                                                    & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__addr 
                                                       >> 2U))] 
                                                   >> 
                                                   (0x18U 
                                                    & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__addr 
                                                       << 3U)))
                    : 0U);
            if ((0U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data 
                    = VL_EXTENDS_II(32,8, (0xffU & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data));
            } else if ((1U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data 
                    = VL_EXTENDS_II(32,16, (0xffffU 
                                            & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data));
            } else if ((4U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data 
                    = (0xffU & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data);
            } else if ((5U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data 
                    = (0xffffU & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data);
            }
            if ((0U != (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__rd))) {
                __Vdlyvval__toplevel__DOT__regs__v2 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk4__DOT__data;
                __Vdlyvset__toplevel__DOT__regs__v2 = 1U;
                __Vdlyvdim0__toplevel__DOT__regs__v2 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__rd;
            }
        } else if ((0x23U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__imm 
                = (((- (IData)((vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                >> 0x1fU))) << 0xbU) 
                   | ((0x7e0U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                 >> 0x14U)) | (0x1fU 
                                               & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                                  >> 7U))));
            __Vdly__pc = ((IData)(4U) + vlSelf->pc);
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                = (vlSelf->toplevel__DOT__regs[vlSelf->toplevel__DOT__unnamedblk2__DOT__r1] 
                   + vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__imm);
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__data 
                = ((0x1fU >= (0x18U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                                       << 3U))) ? (
                                                   vlSelf->toplevel__DOT__regs
                                                   [vlSelf->toplevel__DOT__unnamedblk2__DOT__r2] 
                                                   << 
                                                   (0x18U 
                                                    & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                                                       << 3U)))
                    : 0U);
            vlSelf->bus_write_enable = 1U;
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__mask = 0U;
            if ((0U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__mask 
                    = ((0x1fU >= (0x18U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                                           << 3U)))
                        ? ((IData)(0xffU) << (0x18U 
                                              & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                                                 << 3U)))
                        : 0U);
            }
            if ((1U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__mask 
                    = ((0x1fU >= (0x18U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                                           << 3U)))
                        ? ((IData)(0xffffU) << (0x18U 
                                                & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                                                   << 3U)))
                        : 0U);
            }
            if ((2U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__mask = 0xffffffffU;
            }
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__phys_addr 
                = (0x7fffU & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr 
                              >> 2U));
            __Vdlyvval__toplevel__DOT__data_mem__v0 
                = ((vlSelf->toplevel__DOT__data_mem
                    [vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__phys_addr] 
                    & (~ vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__mask)) 
                   | (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__data 
                      & vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__mask));
            __Vdlyvset__toplevel__DOT__data_mem__v0 = 1U;
            __Vdlyvdim0__toplevel__DOT__data_mem__v0 
                = vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__phys_addr;
            vlSelf->bus_address = vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk5__DOT__addr;
            vlSelf->bus_write_data = vlSelf->toplevel__DOT__regs
                [vlSelf->toplevel__DOT__unnamedblk2__DOT__r2];
        } else if ((0x63U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a 
                = vlSelf->toplevel__DOT__regs[vlSelf->toplevel__DOT__unnamedblk2__DOT__r1];
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b 
                = vlSelf->toplevel__DOT__regs[vlSelf->toplevel__DOT__unnamedblk2__DOT__r2];
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__take_branch 
                = ((0U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                    ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a 
                       == vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b)
                    : ((1U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                        ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a 
                           != vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b)
                        : ((4U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                            ? VL_LTS_III(32, vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a, vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b)
                            : ((5U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                                ? VL_GTES_III(32, vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a, vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b)
                                : ((6U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3))
                                    ? (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a 
                                       < vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b)
                                    : ((7U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__f3)) 
                                       & (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_a 
                                          >= vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__op_b)))))));
            if (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__take_branch) {
                vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__unnamedblk7__DOT__imm 
                    = (((- (IData)((vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                    >> 0x1fU))) << 0xcU) 
                       | ((0x800U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                     << 4U)) | ((0x7e0U 
                                                 & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                                    >> 0x14U)) 
                                                | (0x1eU 
                                                   & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                                      >> 7U)))));
                __Vdly__pc = (vlSelf->pc + vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk6__DOT__unnamedblk7__DOT__imm);
            } else {
                __Vdly__pc = ((IData)(4U) + vlSelf->pc);
            }
        } else if ((0x6fU == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk8__DOT__imm 
                = (((- (IData)((vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                >> 0x1fU))) << 0x14U) 
                   | ((0xff000U & vlSelf->toplevel__DOT__unnamedblk2__DOT__inst) 
                      | ((0x800U & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                    >> 9U)) | ((0x7e0U 
                                                & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                                   >> 0x14U)) 
                                               | (0x1eU 
                                                  & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                                     >> 0x14U))))));
            if ((0U != (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__rd))) {
                __Vdlyvval__toplevel__DOT__regs__v3 
                    = ((IData)(4U) + vlSelf->pc);
                __Vdlyvset__toplevel__DOT__regs__v3 = 1U;
                __Vdlyvdim0__toplevel__DOT__regs__v3 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__rd;
            }
            __Vdly__pc = (vlSelf->pc + vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk8__DOT__imm);
        } else if ((0x67U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk9__DOT__rr1 
                = vlSelf->toplevel__DOT__regs[vlSelf->toplevel__DOT__unnamedblk2__DOT__r1];
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk9__DOT__imm 
                = (((- (IData)((vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                >> 0x1fU))) << 0xbU) 
                   | (0x7ffU & (vlSelf->toplevel__DOT__unnamedblk2__DOT__inst 
                                >> 0x14U)));
            if ((0U != (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__rd))) {
                __Vdlyvval__toplevel__DOT__regs__v4 
                    = ((IData)(4U) + vlSelf->pc);
                __Vdlyvset__toplevel__DOT__regs__v4 = 1U;
                __Vdlyvdim0__toplevel__DOT__regs__v4 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__rd;
            }
            __Vdly__pc = (vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk9__DOT__rr1 
                          + vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk9__DOT__imm);
        } else if ((0x37U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk10__DOT__imm 
                = (0xfffff000U & vlSelf->toplevel__DOT__unnamedblk2__DOT__inst);
            __Vdly__pc = ((IData)(4U) + vlSelf->pc);
            if ((0U != (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__rd))) {
                __Vdlyvval__toplevel__DOT__regs__v5 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk10__DOT__imm;
                __Vdlyvset__toplevel__DOT__regs__v5 = 1U;
                __Vdlyvdim0__toplevel__DOT__regs__v5 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__rd;
            }
        } else if ((0x17U == (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__op))) {
            vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk11__DOT__imm 
                = (0xfffff000U & vlSelf->toplevel__DOT__unnamedblk2__DOT__inst);
            if ((0U != (IData)(vlSelf->toplevel__DOT__unnamedblk2__DOT__rd))) {
                __Vdlyvval__toplevel__DOT__regs__v6 
                    = (vlSelf->pc + vlSelf->toplevel__DOT__unnamedblk2__DOT__unnamedblk11__DOT__imm);
                __Vdlyvset__toplevel__DOT__regs__v6 = 1U;
                __Vdlyvdim0__toplevel__DOT__regs__v6 
                    = vlSelf->toplevel__DOT__unnamedblk2__DOT__rd;
            }
            __Vdly__pc = ((IData)(4U) + vlSelf->pc);
        }
    }
    vlSelf->pc = __Vdly__pc;
    if (__Vdlyvset__toplevel__DOT__data_mem__v0) {
        vlSelf->toplevel__DOT__data_mem[__Vdlyvdim0__toplevel__DOT__data_mem__v0] 
            = __Vdlyvval__toplevel__DOT__data_mem__v0;
    }
    if (__Vdlyvset__toplevel__DOT__regs__v0) {
        vlSelf->toplevel__DOT__regs[0U] = 0U;
    }
    if (__Vdlyvset__toplevel__DOT__regs__v1) {
        vlSelf->toplevel__DOT__regs[__Vdlyvdim0__toplevel__DOT__regs__v1] 
            = __Vdlyvval__toplevel__DOT__regs__v1;
    }
    if (__Vdlyvset__toplevel__DOT__regs__v2) {
        vlSelf->toplevel__DOT__regs[__Vdlyvdim0__toplevel__DOT__regs__v2] 
            = __Vdlyvval__toplevel__DOT__regs__v2;
    }
    if (__Vdlyvset__toplevel__DOT__regs__v3) {
        vlSelf->toplevel__DOT__regs[__Vdlyvdim0__toplevel__DOT__regs__v3] 
            = __Vdlyvval__toplevel__DOT__regs__v3;
    }
    if (__Vdlyvset__toplevel__DOT__regs__v4) {
        vlSelf->toplevel__DOT__regs[__Vdlyvdim0__toplevel__DOT__regs__v4] 
            = __Vdlyvval__toplevel__DOT__regs__v4;
    }
    if (__Vdlyvset__toplevel__DOT__regs__v5) {
        vlSelf->toplevel__DOT__regs[__Vdlyvdim0__toplevel__DOT__regs__v5] 
            = __Vdlyvval__toplevel__DOT__regs__v5;
    }
    if (__Vdlyvset__toplevel__DOT__regs__v6) {
        vlSelf->toplevel__DOT__regs[__Vdlyvdim0__toplevel__DOT__regs__v6] 
            = __Vdlyvval__toplevel__DOT__regs__v6;
    }
}

void Vtoplevel___024root___eval(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval\n"); );
    // Body
    if (((IData)(vlSelf->clock) & (~ (IData)(vlSelf->__Vclklast__TOP__clock)))) {
        Vtoplevel___024root___sequent__TOP__0(vlSelf);
    }
    // Final
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
}

#ifdef VL_DEBUG
void Vtoplevel___024root___eval_debug_assertions(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clock & 0xfeU))) {
        Verilated::overWidthError("clock");}
    if (VL_UNLIKELY((vlSelf->tock_reset & 0xfeU))) {
        Verilated::overWidthError("tock_reset");}
}
#endif  // VL_DEBUG
