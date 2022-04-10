// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtoplevel.h for the primary calling header

#include "verilated.h"

#include "Vtoplevel___024root.h"

VL_ATTR_COLD void Vtoplevel___024root___initial__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___initial__TOP__0\n"); );
    // Init
    VlWide<3>/*95:0*/ __Vtemp_hded71218__0;
    VlWide<3>/*95:0*/ __Vtemp_hfdc9732b__0;
    // Body
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs[0U] = 0U;
    __Vtemp_hded71218__0[0U] = 0x653d2573U;
    __Vtemp_hded71218__0[1U] = 0x5f66696cU;
    __Vtemp_hded71218__0[2U] = 0x74657874U;
    (void)VL_VALUEPLUSARGS_INN(64, VL_CVT_PACK_STR_NW(3, __Vtemp_hded71218__0), 
                               vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__unnamedblk1__DOT__s);VL_READMEM_N(true
                                                                                , 32
                                                                                , 16384
                                                                                , 0
                                                                                , 
                                                                                VL_CVT_PACK_STR_NN(vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__unnamedblk1__DOT__s)
                                                                                ,  &(vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem)
                                                                                , 0
                                                                                , ~0ULL);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value = 0x400000U;
    __Vtemp_hfdc9732b__0[0U] = 0x653d2573U;
    __Vtemp_hfdc9732b__0[1U] = 0x5f66696cU;
    __Vtemp_hfdc9732b__0[2U] = 0x64617461U;
    (void)VL_VALUEPLUSARGS_INN(64, VL_CVT_PACK_STR_NW(3, __Vtemp_hfdc9732b__0), 
                               vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__unnamedblk1__DOT__s);VL_READMEM_N(true
                                                                                , 32
                                                                                , 32768
                                                                                , 0
                                                                                , 
                                                                                VL_CVT_PACK_STR_NN(vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__unnamedblk1__DOT__s)
                                                                                ,  &(vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem)
                                                                                , 0
                                                                                , ~0ULL);
}

extern const VlUnpacked<CData/*0:0*/, 128> Vtoplevel__ConstPool__TABLE_h0f0752ae_0;
extern const VlUnpacked<CData/*0:0*/, 16> Vtoplevel__ConstPool__TABLE_hfd21f17e_0;

VL_ATTR_COLD void Vtoplevel___024root___settle__TOP__0(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___settle__TOP__0\n"); );
    // Init
    IData/*31:0*/ toplevel__DOT__data_memory_bus_q;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath_temp_rs1_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath_temp_rs2_data;
    CData/*2:0*/ toplevel__DOT__core__DOT__ctlpath_inst_funct3;
    CData/*6:0*/ toplevel__DOT__core__DOT__ctlpath_inst_funct7;
    IData/*31:0*/ toplevel__DOT__core__DOT__dmem_address;
    CData/*2:0*/ toplevel__DOT__core__DOT__dmem_data_format;
    IData/*31:0*/ toplevel__DOT__core__DOT__dmem_read_data;
    CData/*6:0*/ toplevel__DOT__core__DOT__unnamedblk1__DOT__opcode;
    CData/*2:0*/ toplevel__DOT__core__DOT__unnamedblk1__DOT__funct3;
    CData/*6:0*/ toplevel__DOT__core__DOT__unnamedblk1__DOT__funct7;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__alu_core_result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_in0;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_in0;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_in2;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_out;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__igen_immediate;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_plus_4;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_plus_imm;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__reg_data;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select__DOT__unnamedblk1__DOT__result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a__DOT__unnamedblk1__DOT__result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b__DOT__unnamedblk1__DOT__result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback__DOT__unnamedblk1__DOT__result;
    IData/*31:0*/ toplevel__DOT__core__DOT__datapath__DOT__igen__DOT__unnamedblk1__DOT__result;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2;
    CData/*2:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__unnamedblk1__DOT__take_branch;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk1__DOT__result;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk2__DOT__result;
    CData/*0:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk3__DOT__result;
    CData/*1:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk4__DOT__result;
    CData/*2:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk5__DOT__result;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_imm_funct;
    CData/*4:0*/ toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__branch_funct;
    CData/*3:0*/ toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__result;
    IData/*31:0*/ toplevel__DOT__core__DOT__dmem__DOT__unnamedblk2__DOT__position_fix;
    IData/*31:0*/ toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__fetched;
    CData/*0:0*/ toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr;
    IData/*31:0*/ toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched;
    CData/*0:0*/ toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory;
    CData/*6:0*/ __Vtableidx1;
    CData/*3:0*/ __Vtableidx2;
    // Body
    vlSelf->o_pc = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_plus_4 
        = ((IData)(4U) + vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value);
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_plus_4;
    toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_in2 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_plus_4;
    toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__fetched 
        = vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem
        [(0x3fffU & (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value 
                     >> 2U))];
    toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr 
        = ((0x400000U <= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value) 
           & (0x40ffffU >= vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value));
    vlSelf->toplevel__DOT__text_memory_bus_read_data 
        = ((IData)(toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__is_text_addr)
            ? toplevel__DOT__text_memory_bus__DOT__unnamedblk1__DOT__fetched
            : 0U);
    vlSelf->o_inst = vlSelf->toplevel__DOT__text_memory_bus_read_data;
    toplevel__DOT__core__DOT__datapath_temp_rs1_data 
        = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
        [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                   >> 0xfU))];
    toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_in0 
        = toplevel__DOT__core__DOT__datapath_temp_rs1_data;
    toplevel__DOT__core__DOT__unnamedblk1__DOT__funct7 
        = (vlSelf->toplevel__DOT__text_memory_bus_read_data 
           >> 0x19U);
    toplevel__DOT__core__DOT__ctlpath_inst_funct7 = toplevel__DOT__core__DOT__unnamedblk1__DOT__funct7;
    toplevel__DOT__core__DOT__datapath_temp_rs2_data 
        = vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs
        [(0x1fU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                   >> 0x14U))];
    toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_in0 
        = toplevel__DOT__core__DOT__datapath_temp_rs2_data;
    toplevel__DOT__core__DOT__datapath__DOT__igen__DOT__unnamedblk1__DOT__result 
        = (((((((((3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)) 
                  | (7U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
                 | (0x13U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
                | (0x67U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
               | (0x27U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
              | (0x23U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
             | (0x63U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))) 
            | (0x17U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data)))
            ? ((3U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                ? (((- (IData)((vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                >> 0x1fU))) << 0xbU) 
                   | (0x7ffU & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                >> 0x14U))) : ((7U 
                                                == 
                                                (0x7fU 
                                                 & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                ? (
                                                   ((- (IData)(
                                                               (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                >> 0x1fU))) 
                                                    << 0xbU) 
                                                   | (0x7ffU 
                                                      & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                         >> 0x14U)))
                                                : (
                                                   (0x13U 
                                                    == 
                                                    (0x7fU 
                                                     & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                    ? 
                                                   (((- (IData)(
                                                                (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                 >> 0x1fU))) 
                                                     << 0xbU) 
                                                    | (0x7ffU 
                                                       & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                          >> 0x14U)))
                                                    : 
                                                   ((0x67U 
                                                     == 
                                                     (0x7fU 
                                                      & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                     ? 
                                                    (((- (IData)(
                                                                 (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                  >> 0x1fU))) 
                                                      << 0xbU) 
                                                     | (0x7ffU 
                                                        & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                           >> 0x14U)))
                                                     : 
                                                    ((0x27U 
                                                      == 
                                                      (0x7fU 
                                                       & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                      ? 
                                                     (((- (IData)(
                                                                  (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                   >> 0x1fU))) 
                                                       << 0xbU) 
                                                      | ((0x7e0U 
                                                          & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                             >> 0x14U)) 
                                                         | (0x1fU 
                                                            & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                               >> 7U))))
                                                      : 
                                                     ((0x23U 
                                                       == 
                                                       (0x7fU 
                                                        & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                       ? 
                                                      (((- (IData)(
                                                                   (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                    >> 0x1fU))) 
                                                        << 0xbU) 
                                                       | ((0x7e0U 
                                                           & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                              >> 0x14U)) 
                                                          | (0x1fU 
                                                             & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                >> 7U))))
                                                       : 
                                                      ((0x63U 
                                                        == 
                                                        (0x7fU 
                                                         & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                                                        ? 
                                                       (((- (IData)(
                                                                    (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                     >> 0x1fU))) 
                                                         << 0xcU) 
                                                        | ((0x800U 
                                                            & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                               << 4U)) 
                                                           | ((0x7e0U 
                                                               & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                  >> 0x14U)) 
                                                              | (0x1eU 
                                                                 & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                                    >> 7U)))))
                                                        : 
                                                       (0xfffff000U 
                                                        & vlSelf->toplevel__DOT__text_memory_bus_read_data))))))))
            : ((0x37U == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                ? (0xfffff000U & vlSelf->toplevel__DOT__text_memory_bus_read_data)
                : ((0x6fU == (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data))
                    ? (((- (IData)((vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                    >> 0x1fU))) << 0x14U) 
                       | ((0xff000U & vlSelf->toplevel__DOT__text_memory_bus_read_data) 
                          | ((0x800U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                        >> 9U)) | (
                                                   (0x7e0U 
                                                    & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                       >> 0x14U)) 
                                                   | (0x1eU 
                                                      & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                                                         >> 0x14U))))))
                    : 0U)));
    toplevel__DOT__core__DOT__datapath__DOT__igen_immediate 
        = toplevel__DOT__core__DOT__datapath__DOT__igen__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__unnamedblk1__DOT__funct3 
        = (7U & (vlSelf->toplevel__DOT__text_memory_bus_read_data 
                 >> 0xcU));
    toplevel__DOT__core__DOT__ctlpath_inst_funct3 = toplevel__DOT__core__DOT__unnamedblk1__DOT__funct3;
    toplevel__DOT__core__DOT__dmem_data_format = toplevel__DOT__core__DOT__unnamedblk1__DOT__funct3;
    toplevel__DOT__core__DOT__unnamedblk1__DOT__opcode 
        = (0x7fU & vlSelf->toplevel__DOT__text_memory_bus_read_data);
    vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode 
        = toplevel__DOT__core__DOT__unnamedblk1__DOT__opcode;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_plus_imm 
        = (vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value 
           + toplevel__DOT__core__DOT__datapath__DOT__igen_immediate);
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_plus_imm;
    vlSelf->o_bus_write_enable = (0x23U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode));
    vlSelf->o_bus_read_enable = (3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode));
    toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk2__DOT__result 
        = (((((((((0xfU == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
                  | (0x23U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
                 | (0x63U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
                | (3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
               | (0x13U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
              | (0x17U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
             | (0x33U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
            | (0x37U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)))
            ? ((0xfU != (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
               & ((0x23U != (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
                  & (0x63U != (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))))
            : ((0x67U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
               | (0x6fU == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))));
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk2__DOT__result;
    toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk5__DOT__result 
        = ((0x13U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
            ? 0U : ((0x17U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                     ? 0U : ((0x33U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                              ? 0U : ((0x37U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                       ? 3U : ((0x67U 
                                                == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                                ? 2U
                                                : (
                                                   (0x6fU 
                                                    == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                                    ? 2U
                                                    : 
                                                   ((3U 
                                                     == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                                     ? 1U
                                                     : 0U)))))));
    toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk5__DOT__result;
    toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk3__DOT__result 
        = (((((((((0x17U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
                  | (0x6fU == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
                 | (0x33U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
                | (0x37U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
               | (0x63U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
              | (3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
             | (0x23U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
            | (0x13U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
           & ((0x17U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
              | (0x6fU == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))));
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk3__DOT__result;
    __Vtableidx1 = vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select 
        = Vtoplevel__ConstPool__TABLE_h0f0752ae_0[__Vtableidx1];
    toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk4__DOT__result 
        = (((((((((0x17U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
                  | (0x6fU == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
                 | (0x33U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
                | (0x63U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
               | (3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
              | (0x23U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
             | (0x13U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))) 
            | (0x67U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)))
            ? ((0x17U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                ? 0U : ((0x6fU == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                         ? 0U : ((0x33U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                  ? 2U : ((0x63U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                           ? 1U : (
                                                   (3U 
                                                    == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                                    ? 0U
                                                    : 
                                                   ((0x23U 
                                                     == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                                     ? 0U
                                                     : 
                                                    ((0x13U 
                                                      == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                                      ? 3U
                                                      : 0U)))))))
            : 0U);
    toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk4__DOT__result;
    toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a__DOT__unnamedblk1__DOT__result 
        = ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
            ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_a_select)
                ? vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value
                : 0U) : toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_in0);
    toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b__DOT__unnamedblk1__DOT__result 
        = ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
            ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_operand_b_select)
                ? toplevel__DOT__core__DOT__datapath__DOT__igen_immediate
                : 0U) : toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_in0);
    toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct 
        = (((((((((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3)) 
                  | (1U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
                 | (2U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
                | (3U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
               | (4U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
              | (5U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
             | (6U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))) 
            | (7U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3)))
            ? ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                ? 1U : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                         ? 3U : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                  ? 7U : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                           ? 8U : (
                                                   (4U 
                                                    == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                    ? 9U
                                                    : 
                                                   ((5U 
                                                     == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                     ? 4U
                                                     : 
                                                    ((6U 
                                                      == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                      ? 0xaU
                                                      : 0xbU)))))))
            : 0U);
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
            ? 2U : ((5U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                     ? 5U : 0U));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_funct 
        = ((0x20U & (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct7))
            ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct)
            : (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_imm_funct 
        = ((((IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct7) 
             >> 5U) & (1U == (3U & (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))))
            ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__secondary_funct)
            : (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__default_funct));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__branch_funct 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
            ? 6U : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                     ? 6U : ((4U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                              ? 7U : ((5U == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                       ? 7U : ((6U 
                                                == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                ? 8U
                                                : (
                                                   (7U 
                                                    == (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3))
                                                    ? 8U
                                                    : 0U))))));
    toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
            ? 1U : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                     ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_funct)
                     : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                         ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__op_imm_funct)
                         : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_alu_op_type2))
                             ? (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl__DOT__branch_funct)
                             : 0U))));
    toplevel__DOT__core__DOT__datapath__DOT__alu_core_result 
        = (((((((((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function)) 
                  | (2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
                 | (3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
                | (4U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
               | (5U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
              | (6U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
             | (7U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))) 
            | (8U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function)))
            ? ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                   + toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out)
                : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                    ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                       - toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out)
                    : ((3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                        ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                           << (0x1fU & toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out))
                        : ((4U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                            ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                               >> (0x1fU & toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out))
                            : ((5U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                                ? VL_SHIFTRS_III(32,32,5, toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out, 
                                                 (0x1fU 
                                                  & toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out))
                                : ((6U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                                    ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                                       == toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out)
                                    : ((7U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                                        ? VL_LTS_III(32, toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out, toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out)
                                        : (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                                           < toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out))))))))
            : ((9U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                   ^ toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out)
                : ((0xaU == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                    ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                       | toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out)
                    : ((0xbU == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__alu_ctrl_alu_function))
                        ? (toplevel__DOT__core__DOT__datapath__DOT__mux_operand_a_out 
                           & toplevel__DOT__core__DOT__datapath__DOT__mux_operand_b_out)
                        : 0U))));
    vlSelf->toplevel__DOT__core_alu_result = toplevel__DOT__core__DOT__datapath__DOT__alu_core_result;
    toplevel__DOT__core__DOT__dmem_address = vlSelf->toplevel__DOT__core_alu_result;
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data 
        = ((0x1fU >= (0x18U & (toplevel__DOT__core__DOT__dmem_address 
                               << 3U))) ? (toplevel__DOT__core__DOT__datapath_temp_rs2_data 
                                           << (0x18U 
                                               & (toplevel__DOT__core__DOT__dmem_address 
                                                  << 3U)))
            : 0U);
    toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__result 
        = (0xfU & ((0U == (3U & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                    ? ((IData)(1U) << (3U & toplevel__DOT__core__DOT__dmem_address))
                    : ((1U == (3U & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                        ? ((IData)(3U) << (3U & toplevel__DOT__core__DOT__dmem_address))
                        : ((2U == (3U & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                            ? ((IData)(0xfU) << (3U 
                                                 & toplevel__DOT__core__DOT__dmem_address))
                            : 0U))));
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable 
        = toplevel__DOT__core__DOT__dmem__DOT__unnamedblk1__DOT__result;
    vlSelf->o_bus_address = vlSelf->toplevel__DOT__core_alu_result;
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched 
        = vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem
        [(0x7fffU & (vlSelf->toplevel__DOT__core_alu_result 
                     >> 2U))];
    toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory 
        = ((0x80000000U <= vlSelf->toplevel__DOT__core_alu_result) 
           & (0x8001ffffU >= vlSelf->toplevel__DOT__core_alu_result));
    toplevel__DOT__data_memory_bus_q = (((3U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode)) 
                                         & (IData)(toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__is_data_memory))
                                         ? toplevel__DOT__data_memory_bus__DOT__unnamedblk1__DOT__fetched
                                         : 0U);
    __Vtableidx2 = (((0U == vlSelf->toplevel__DOT__core_alu_result) 
                     << 3U) | (IData)(toplevel__DOT__core__DOT__ctlpath_inst_funct3));
    toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch 
        = Vtoplevel__ConstPool__TABLE_hfd21f17e_0[__Vtableidx2];
    vlSelf->o_bus_write_data = vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data;
    vlSelf->o_bus_byte_enable = vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable;
    vlSelf->o_bus_read_data = toplevel__DOT__data_memory_bus_q;
    toplevel__DOT__core__DOT__dmem__DOT__unnamedblk2__DOT__position_fix 
        = ((0x1fU >= (0x18U & (toplevel__DOT__core__DOT__dmem_address 
                               << 3U))) ? (toplevel__DOT__data_memory_bus_q 
                                           >> (0x18U 
                                               & (toplevel__DOT__core__DOT__dmem_address 
                                                  << 3U)))
            : 0U);
    toplevel__DOT__core__DOT__dmem_read_data = ((0U 
                                                 == 
                                                 (3U 
                                                  & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                 ? 
                                                (((- (IData)(
                                                             (1U 
                                                              & ((~ 
                                                                  ((IData)(toplevel__DOT__core__DOT__dmem_data_format) 
                                                                   >> 2U)) 
                                                                 & (toplevel__DOT__core__DOT__dmem__DOT__unnamedblk2__DOT__position_fix 
                                                                    >> 7U))))) 
                                                  << 8U) 
                                                 | (0xffU 
                                                    & toplevel__DOT__core__DOT__dmem__DOT__unnamedblk2__DOT__position_fix))
                                                 : 
                                                ((1U 
                                                  == 
                                                  (3U 
                                                   & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                  ? 
                                                 (((- (IData)(
                                                              (1U 
                                                               & ((~ 
                                                                   ((IData)(toplevel__DOT__core__DOT__dmem_data_format) 
                                                                    >> 2U)) 
                                                                  & (toplevel__DOT__core__DOT__dmem__DOT__unnamedblk2__DOT__position_fix 
                                                                     >> 0xfU))))) 
                                                   << 0x10U) 
                                                  | (0xffffU 
                                                     & toplevel__DOT__core__DOT__dmem__DOT__unnamedblk2__DOT__position_fix))
                                                  : 
                                                 ((2U 
                                                   == 
                                                   (3U 
                                                    & (IData)(toplevel__DOT__core__DOT__dmem_data_format)))
                                                   ? toplevel__DOT__core__DOT__dmem__DOT__unnamedblk2__DOT__position_fix
                                                   : 0U)));
    toplevel__DOT__core__DOT__ctlpath__DOT__unnamedblk1__DOT__take_branch 
        = toplevel__DOT__core__DOT__ctlpath__DOT__transfer_take_branch;
    toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch 
        = toplevel__DOT__core__DOT__ctlpath__DOT__unnamedblk1__DOT__take_branch;
    toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback__DOT__unnamedblk1__DOT__result 
        = (((((((((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select)) 
                  | (1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                 | (2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
                | (3U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
               | (4U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
              | (5U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
             | (6U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))) 
            | (7U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select)))
            ? ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))
                ? toplevel__DOT__core__DOT__datapath__DOT__alu_core_result
                : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))
                    ? toplevel__DOT__core__DOT__dmem_read_data
                    : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_reg_writeback_select))
                        ? toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_in2
                        : toplevel__DOT__core__DOT__datapath__DOT__igen_immediate)))
            : 0U);
    toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_out 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk1__DOT__result 
        = (3U & ((0x63U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                  ? ((IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_take_branch)
                      ? 1U : 0U) : ((0x67U == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                     ? 2U : ((0x6fU 
                                              == (IData)(vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode))
                                              ? 1U : 0U))));
    toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select 
        = toplevel__DOT__core__DOT__ctlpath__DOT__control__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__reg_data 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_reg_writeback_out;
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs_rd_data 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__reg_data;
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select__DOT__unnamedblk1__DOT__result 
        = ((0U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select))
            ? toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in0
            : ((1U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select))
                ? toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_in1
                : ((2U == (IData)(toplevel__DOT__core__DOT__ctlpath__DOT__control_next_pc_select))
                    ? (0xfffffffeU & toplevel__DOT__core__DOT__datapath__DOT__alu_core_result)
                    : 0U)));
    toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select__DOT__unnamedblk1__DOT__result;
    toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_data 
        = toplevel__DOT__core__DOT__datapath__DOT__mux_next_pc_select_out;
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_next 
        = toplevel__DOT__core__DOT__datapath__DOT__unnamedblk1__DOT__pc_data;
}

VL_ATTR_COLD void Vtoplevel___024root___eval_initial(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_initial\n"); );
    // Body
    Vtoplevel___024root___initial__TOP__0(vlSelf);
    vlSelf->__Vclklast__TOP__clock = vlSelf->clock;
}

VL_ATTR_COLD void Vtoplevel___024root___eval_settle(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___eval_settle\n"); );
    // Body
    Vtoplevel___024root___settle__TOP__0(vlSelf);
}

VL_ATTR_COLD void Vtoplevel___024root___final(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___final\n"); );
}

VL_ATTR_COLD void Vtoplevel___024root___ctor_var_reset(Vtoplevel___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtoplevel__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtoplevel___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clock = VL_RAND_RESET_I(1);
    vlSelf->reset = VL_RAND_RESET_I(1);
    vlSelf->o_bus_read_data = VL_RAND_RESET_I(32);
    vlSelf->o_bus_address = VL_RAND_RESET_I(32);
    vlSelf->o_bus_write_data = VL_RAND_RESET_I(32);
    vlSelf->o_bus_byte_enable = VL_RAND_RESET_I(4);
    vlSelf->o_bus_read_enable = VL_RAND_RESET_I(1);
    vlSelf->o_bus_write_enable = VL_RAND_RESET_I(1);
    vlSelf->o_inst = VL_RAND_RESET_I(32);
    vlSelf->o_pc = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core_alu_result = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__text_memory_bus_read_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__ctlpath_inst_opcode = VL_RAND_RESET_I(7);
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_write_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__dmem_bus_byte_enable = VL_RAND_RESET_I(4);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter_next = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs_rd_data = VL_RAND_RESET_I(32);
    vlSelf->toplevel__DOT__core__DOT__datapath__DOT__program_counter__DOT___value = VL_RAND_RESET_I(32);
    for (int __Vi0=0; __Vi0<32; ++__Vi0) {
        vlSelf->toplevel__DOT__core__DOT__datapath__DOT__regs__DOT__regs[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->toplevel__DOT__core__DOT__ctlpath__DOT__control_regfile_write_enable = VL_RAND_RESET_I(1);
    for (int __Vi0=0; __Vi0<16384; ++__Vi0) {
        vlSelf->toplevel__DOT__text_memory_bus__DOT__text_memory__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    for (int __Vi0=0; __Vi0<32768; ++__Vi0) {
        vlSelf->toplevel__DOT__data_memory_bus__DOT__data_memory__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    }
