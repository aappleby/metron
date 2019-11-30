#pragma once

//-----------------------------------------------------------------------------

typedef const bool wire;

struct Reg {
  void tock(wire rst_in, wire clk_in, wire reg_in) {
    if (!rst_in) val = 0;
    else if (!clk && clk_in) val = reg_in;
    clk = clk_in;
  }

  operator bool() const { return val; }

  bool val;
  bool clk;
};

//-----------------------------------------------------------------------------

struct ChipIn {
  bool RST;
  bool CLKIN_A;
  bool CLKIN_B;
  bool T2;
  bool T1;
  bool WR_C;
  bool RD_C;
};

struct CpuIn {
  bool CPU_RAW_RD;
  bool CPU_RAW_WR;
  bool ADDR_VALID;
  bool CPUCLK_REQ;
};

struct CpuOut {
  bool CPU_RESET;
};

//-----------------------------------------------------------------------------

struct System {

  void tick(const CpuIn& cpu_in, const ChipIn& chip_in);
  void tock(const CpuIn& cpu_in, const ChipIn& chip_in);

  bool T1n;
  bool T2n;
  bool MODE_PROD;
  bool MODE_DBG1;
  bool MODE_DBG2;
  bool DIV_CLK;

  //----------
  // wires

  bool SYS_RESET4;
  bool SYS_RESETn1;
  bool SYS_RESET1;
  bool SYS_RESET2;
  bool SYS_RESETn2;
  bool SYS_RESETn3;
  bool SYS_RESET3;
  bool SYS_RESETn4;
  bool SYS_RESETn5;
  bool SYS_RESETn6;
  bool SYS_RESETn7;

  //----------
  // regs

  Reg DIV_15;
  Reg RESET_REG;
};

//-----------------------------------------------------------------------------
