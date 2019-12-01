#include "MetronTest1.h"

#if 0
struct Foo {
  int signalA, signalB, signalC;
  int stateA, stateB, stateC;

  int other;

  void resetEverything() {
    stateA = 0;
    stateB = 0;
    stateC = 1;
  }

  void tickStuff() {
    signalA = 1;
    signalB = signalA * 2;
    signalC = signalB * 7;
  }

  void tockOtherStuff() {
    stateA = signalA;
    stateB = signalB;
    stateC = signalC;
  }
};
#endif

#if 1
wire _not(wire a)                   { return !a; }
wire _and(wire a, wire b)           { return a && b; }
wire _or (wire a, wire b)           { return a || b; }
wire _nor(wire a, wire b)           { return !(a || b); }
wire _or (wire a, wire b, wire c)   { return a || b || c; }

//-----------------------------------------------------------------------------

void System::tick(const CpuIn& cpu_in, const ChipIn& chip_in) {

  //----------
  // CPU reset

  wire CLK_BAD1    = _not(chip_in.CLKIN_A);
  wire CPUCLK_REQn = _not(cpu_in.CPUCLK_REQ);

  wire NO_CLOCK    = _or(CPUCLK_REQn, /*p01.UPYF*/ _or(chip_in.RST, CLK_BAD1));
  wire TIMEOUT     = _and(NO_CLOCK, DIV_15);
  wire CPU_RESET   = _or(MODE_DBG2, MODE_DBG1, TIMEOUT);
  wire CPU_RESETn  = _not(CPU_RESET);

  //----------
  // SYS reset tree

  wire RESET_IN  = _or (/*p01.AFAR*/ _nor(CPU_RESETn, chip_in.RST), chip_in.RST);

  SYS_RESET4  = _or(RESET_REG, RESET_IN);
  SYS_RESETn1 = _not(SYS_RESET4);
  SYS_RESET1  = _not(SYS_RESETn1);
  SYS_RESETn2 = _not(SYS_RESET1);
  SYS_RESET3  = _not(SYS_RESETn2);
  SYS_RESETn4 = _not(SYS_RESET3);
  SYS_RESETn5 = _not(SYS_RESET3);
  SYS_RESETn6 = _not(SYS_RESET3);
  SYS_RESETn7 = _not(SYS_RESET3);
  SYS_RESET2  = _not(SYS_RESETn1);
  SYS_RESETn3 = _not(SYS_RESET2);
}

//-----------------------------------------------------------------------------

void System::tock(const CpuIn& cpu_in, const ChipIn& chip_in) {
  wire CLK_BAD1    = _not(chip_in.CLKIN_A);
  wire CPUCLK_REQn = _not(cpu_in.CPUCLK_REQ);

  wire NO_CLOCK    = _or(CPUCLK_REQn, _or(chip_in.RST, CLK_BAD1));
  wire TIMEOUT     = _and(NO_CLOCK, DIV_15);
  wire CPU_RESET   = _or(MODE_DBG2, MODE_DBG1, TIMEOUT);
  wire CPU_RESETn  = _not(CPU_RESET);

  wire RESET_CLK_IN = _not(DIV_CLK);
  wire RESET_REG_IN = _or(_nor(CPU_RESETn, chip_in.RST), chip_in.RST);

  //RESET_REG.tock(_not(DIV_CLK), MODE_PROD, RESET_REG_IN);
}

//-----------------------------------------------------------------------------
#endif