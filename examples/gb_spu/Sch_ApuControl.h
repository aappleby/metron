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

  //----------
  // clocks for the apu

  // DFF17_01 SC
  // DFF17_02 << CLKp
  // DFF17_03 SC
  // DFF17_04 --
  // DFF17_05 --
  // DFF17_06 << RSTn  // must be RSTn, see WUVU/VENA/WOSU
  // DFF17_07 << D
  // DFF17_08 --
  // DFF17_09 SC
  // DFF17_10 --
  // DFF17_11 --
  // DFF17_12 SC
  // DFF17_13 << RSTn
  // DFF17_14 --
  // DFF17_15 --
  // DFF17_16 >> QN   _MUST_ be QN - see TERO
  // DFF17_17 >> Q    _MUST_ be Q  - see TERO

  /*_p01.CERY*/ DFF17 CERY_2M;
  /*_p01.ATYK*/ DFF17 ATYK_2M;
  /*_p01.AVOK*/ DFF17 AVOK_1M;
  /*_p09.AJER*/ DFF17 AJER_2M;
  /*_p01.JESO*/ DFF17 JESO_CLK_512K;

  /*_p01.BARA*/ DFF17 BARA_CLK_512;
  /*_p01.CARU*/ DFF17 CARU_CLK_256;
  /*_p01.BYLU*/ DFF17 BYLU_CLK_128;

  //----------
  // FF24 NR50
  
  /*_p09.APEG*/ DFF9 APEG_NR50_VOL_L0;
  /*_p09.BYGA*/ DFF9 BYGA_NR50_VOL_L1;
  /*_p09.AGER*/ DFF9 AGER_NR50_VOL_L2;
  /*_p09.APOS*/ DFF9 APOS_NR50_VIN_TO_L;
  /*_p09.BYRE*/ DFF9 BYRE_NR50_VOL_R0;
  /*_p09.BUMO*/ DFF9 BUMO_NR50_VOL_R1;
  /*_p09.COZU*/ DFF9 COZU_NR50_VOL_R2;
  /*_p09.BEDU*/ DFF9 BEDU_NR50_VIN_TO_R;

  //----------
  // FF25 NR51

  /*_p09.ANEV*/ DFF9 ANEV_NR51_D0; // these are mixer control bits
  /*_p09.BOGU*/ DFF9 BOGU_NR51_D1;
  /*_p09.BAFO*/ DFF9 BAFO_NR51_D2;
  /*_p09.ATUF*/ DFF9 ATUF_NR51_D3;
  /*_p09.BUME*/ DFF9 BUME_NR51_D4;
  /*_p09.BOFA*/ DFF9 BOFA_NR51_D5;
  /*_p09.BEFO*/ DFF9 BEFO_NR51_D6;
  /*_p09.BEPU*/ DFF9 BEPU_NR51_D7;

  //----------
  // FF26 NR52

  // DBG_APUn polarity?
  /*_p09.FERO*/ DFF9  FERO_NR52_DBG_APUn;     // secret debug bit
  /*_p09.BOWY*/ DFF17 BOWY_NR52_DBG_SWEEP;    // secret debug bit
  /*_p09.HADA*/ DFF17 HADA_NR52_ALL_SOUND_ON; //FF26_7
};

//-----------------------------------------------------------------------------
