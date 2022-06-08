/// plait_noparse

#include "regs.h"

#pragma once

//-----------------------------------------------------------------------------

struct Gameboy;

struct Channel2 {

  void tick();

  /*_p15.DANE*/ NorLatch DANE_CH2_ACTIVE; // nor latch
};

//-----------------------------------------------------------------------------
