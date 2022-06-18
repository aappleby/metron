/// plait_noparse

#pragma once

#include "gates.h"
#include "regs.h"

#include "Sch_Channel1.h"
#include "Sch_Channel2.h"
#include "Sch_Channel3.h"
#include "Sch_Channel4.h"

//-----------------------------------------------------------------------------

struct ApuControl {

  void tick();

  Channel1 ch1;
  Channel2 ch2;
  Channel3 ch3;
  Channel4 ch4;
};

//-----------------------------------------------------------------------------
