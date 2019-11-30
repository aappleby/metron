#include "MetronTest1.h"

wire not(wire a)                   { return !a; }
wire and(wire a, wire b)           { return a && b; }
wire or (wire a, wire b)           { return a || b; }
wire nor(wire a, wire b)           { return !(a || b); }
wire or (wire a, wire b, wire c)   { return a || b || c; }

//-----------------------------------------------------------------------------

void System::tick(const CpuIn& cpu_in, const ChipIn& chip_in) {

  //----------
  // CPU reset

  wire CLK_BAD1    = not(chip_in.CLKIN_A);
  wire CPUCLK_REQn = not(cpu_in.CPUCLK_REQ);

  wire NO_CLOCK    = or(CPUCLK_REQn, /*p01.UPYF*/ or(chip_in.RST, CLK_BAD1));
  wire TIMEOUT     = and(NO_CLOCK, DIV_15);
  wire CPU_RESET   = or(MODE_DBG2, MODE_DBG1, TIMEOUT);
  wire CPU_RESETn  = not(CPU_RESET);

  //----------
  // SYS reset tree

  wire RESET_IN  = or (/*p01.AFAR*/ nor(CPU_RESETn, chip_in.RST), chip_in.RST);

  SYS_RESET4  = or(RESET_REG, RESET_IN);
  SYS_RESETn1 = not(SYS_RESET4);
  SYS_RESET1  = not(SYS_RESETn1);
  SYS_RESETn2 = not(SYS_RESET1);
  SYS_RESET3  = not(SYS_RESETn2);
  SYS_RESETn4 = not(SYS_RESET3);
  SYS_RESETn5 = not(SYS_RESET3);
  SYS_RESETn6 = not(SYS_RESET3);
  SYS_RESETn7 = not(SYS_RESET3);
  SYS_RESET2  = not(SYS_RESETn1);
  SYS_RESETn3 = not(SYS_RESET2);
}

//-----------------------------------------------------------------------------

void System::tock(const CpuIn& cpu_in, const ChipIn& chip_in) {
  wire CLK_BAD1    = not(chip_in.CLKIN_A);
  wire CPUCLK_REQn = not(cpu_in.CPUCLK_REQ);

  wire NO_CLOCK    = or(CPUCLK_REQn, or(chip_in.RST, CLK_BAD1));
  wire TIMEOUT     = and(NO_CLOCK, DIV_15);
  wire CPU_RESET   = or(MODE_DBG2, MODE_DBG1, TIMEOUT);
  wire CPU_RESETn  = not(CPU_RESET);

  wire RESET_CLK_IN = not(DIV_CLK);
  wire RESET_REG_IN = or(nor(CPU_RESETn, chip_in.RST), chip_in.RST);

  RESET_REG.tock(not(DIV_CLK), MODE_PROD, RESET_REG_IN);
}

//-----------------------------------------------------------------------------
