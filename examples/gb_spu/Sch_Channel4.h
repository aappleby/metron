/// plait_noparse

#ifndef SCH_CHANNEL4_H
#define SCH_CHANNEL4_H

#include "metron_tools.h"
#include "regs.h"

//-----------------------------------------------------------------------------

// Noise
// Length counter
// Env vol / dir / period
// Clock shift
// LFSR mode
// LFSR divisor
// START
// STOP

struct Channel4 {

  void tick(logic<16> addr);

  //----------
  // Clock/reset

  //----------
  // FF20 NR41 ch4 length

  // void dff20(wire CLKn, wire LOADp, wire newD)
  /*_p19.DANO*/ DFF20 DANO_NR41_LEN0; 
  /*_p19.FAVY*/ DFF20 FAVY_NR41_LEN1;
  /*_p19.DENA*/ DFF20 DENA_NR41_LEN2;
  /*_p19.CEDO*/ DFF20 CEDO_NR41_LEN3;
  /*_p19.FYLO*/ DFF20 FYLO_NR41_LEN4;
  /*_p19.EDOP*/ DFF20 EDOP_NR41_LEN5;

  //----------
  // FF21 NR42 ch4 envelope

  /*_p19.DYKE*/ logic<1> DYKE_FF21_WRn;
  /*_p19.FUPA*/ logic<1> FUPA_FF21_WRo;

  /*_p19.EMOK*/ logic<1> EMOK_NR42_ENV_TIMER0;
  /*_p19.ETYJ*/ logic<1> ETYJ_NR42_ENV_TIMER1;
  /*_p19.EZYK*/ logic<1> EZYK_NR42_ENV_TIMER2;
  /*_p19.GEKY*/ logic<1> GEKY_NR42_ENV_DIR;
  /*_p19.GARU*/ logic<1> GARU_NR42_ENV_VOL0;
  /*_p19.GOKY*/ logic<1> GOKY_NR42_ENV_VOL1;
  /*_p19.GOZO*/ logic<1> GOZO_NR42_ENV_VOL2;
  /*_p19.GEDU*/ logic<1> GEDU_NR42_ENV_VOL3;

  //----------
  // FF22 NR43 ch4 freq

  /*_p19.HOVA*/ logic<1> HOVA_FF22_WRo;
  /*_p19.HOSO*/ logic<1> HOSO_FF22_WRp;
  /*_p19.EFUG*/ logic<1> EFUG_FF22_WRq;

  /*_p19.JARE*/ logic<1> JARE_NR43_DIV0;
  /*_p19.JERO*/ logic<1> JERO_NR43_DIV1;
  /*_p19.JAKY*/ logic<1> JAKY_NR43_DIV2;
  /*_p19.JAMY*/ logic<1> JAMY_NR43_MODE;
  /*_p19.FETA*/ logic<1> FETA_NR43_FREQ0;
  /*_p19.FYTO*/ logic<1> FYTO_NR43_FREQ1;
  /*_p19.GOGO*/ logic<1> GOGO_NR43_FREQ2;
  /*_p19.GAFO*/ logic<1> GAFO_NR43_FREQ3;

  //---------
  // FF23

  /*_p19.DULU*/ logic<1> DULU_FF23_WRn;
  /*_p19.FOXE*/ logic<1> FOXE_FF23_WRo;

  /*_p19.CUNY*/ DFF9 CUNY_NR44_STOP;
  /*_p19.HOGA*/ logic<1> HOGA_NR44_START;

  //----------
  // Control

  /*_p20.GYSU*/ logic<1> GYSU_CH4_START;
  /*_p20.GENA*/ NorLatch GENA_CH4_ACTIVE;


  /*_p20.GONE*/ logic<1> GONE_RESTART1;
  /*_p20.GORA*/ logic<1> GORA_RESTART2;
  /*_p20.GATY*/ logic<1> GATY_RESTART3;

  /*_p20.KANU*/ logic<1> KANU_DIV_CLKa;

  /*_p20.JYCO*/ logic<1> JYCO_DIV0;
  /*_p20.JYRE*/ logic<1> JYRE_DIV1;
  /*_p20.JYFU*/ logic<1> JYFU_DIV2;
  /*_p20.HYNO*/ logic<1> HYNO_DIV_MAX;

  //----------
  // Length

  /*_p19.FUGO*/ DFF17 FUGO_LEN_STOP;

  //----------
  // Frequency timer

  /*_p20.GYBA*/ logic<1> GYBA_FREQ_GATE_CLK;
  /*_p20.GARY*/ logic<1> GARY_FREQ_GATEn;
  /*_p20.CARY*/ logic<1> CARY_FREQ_CLK;
  /*_p20.CEXO*/ logic<1> CEXO_FREQ_00;
  /*_p20.DEKO*/ logic<1> DEKO_FREQ_01;
  /*_p20.EZEF*/ logic<1> EZEF_FREQ_02;
  /*_p20.EPOR*/ logic<1> EPOR_FREQ_03;
  /*_p20.DURE*/ logic<1> DURE_FREQ_04;
  /*_p20.DALE*/ logic<1> DALE_FREQ_05;
  /*_p20.DOKE*/ logic<1> DOKE_FREQ_06;
  /*_p20.DEMO*/ logic<1> DEMO_FREQ_07;
  /*_p20.DOSE*/ logic<1> DOSE_FREQ_08;
  /*_p20.DETE*/ logic<1> DETE_FREQ_09;
  /*_p20.ERUT*/ logic<1> ERUT_FREQ_10;
  /*_p20.DOTA*/ logic<1> DOTA_FREQ_11;
  /*_p20.DERE*/ logic<1> DERE_FREQ_12;
  /*_p20.ESEP*/ logic<1> ESEP_FREQ_13;

  //----------
  // LFSR

  /*_p20.FEME*/ logic<1> FEME_LFSR_CLKa;
  /*_p20.GYVE*/ logic<1> GYVE_LFSR_CLKb;
  /*_p20.KOPA*/ logic<1> KOPA_LFSR_CLKc;
  /*_p20.JYJA*/ logic<1> JYJA_LFSR_CLKn;
  /*_p20.GUFA*/ logic<1> GUFA_LFSR_CLKo;
  /*_p20.KARA*/ logic<1> KARA_LFSR_CLKp;


  /*_p20.JOTO*/ logic<1> JOTO_LFSR_00;
  /*_p20.KOMU*/ logic<1> KOMU_LFSR_01;
  /*_p20.KETU*/ logic<1> KETU_LFSR_02;
  /*_p20.KUTA*/ logic<1> KUTA_LFSR_03;
  /*_p20.KUZY*/ logic<1> KUZY_LFSR_04;
  /*_p20.KYWY*/ logic<1> KYWY_LFSR_05;
  /*_p20.JAJU*/ logic<1> JAJU_LFSR_06;
  /*_p20.HAPE*/ logic<1> HAPE_LFSR_07;
  /*_p20.JUXE*/ logic<1> JUXE_LFSR_08;
  /*_p20.JEPE*/ logic<1> JEPE_LFSR_09;
  /*_p20.JAVO*/ logic<1> JAVO_LFSR_10;
  /*_p20.HEPA*/ logic<1> HEPA_LFSR_11;
  /*_p20.HORY*/ logic<1> HORY_LFSR_12;
  /*_p20.HENO*/ logic<1> HENO_LFSR_13;
  /*_p20.HYRO*/ logic<1> HYRO_LFSR_14;
  /*_p20.HEZU*/ logic<1> HEZU_LFSR_15;

  //----------
  // Env

  /*_p20.ALOP*/ logic<1> ALOP_CLK_128n;
  /*_p20.ABEL*/ logic<1> ABEL_ENV_TIMER_CLKa;
  /*_p20.BUXO*/ logic<1> BUXO_ENV_TIMER_CLKb;
  /*_p20.CUNA*/ logic<1> CUNA_ENV_TIMER0;
  /*_p20.COFE*/ logic<1> COFE_ENV_TIMER1;
  /*_p20.DOGO*/ logic<1> DOGO_ENV_TIMER2;

  /*_p20.FOSY*/ logic<1> FOSY_ENV_PULSE;
  /*_p20.FYNO*/ logic<1> FYNO_ENV_STOP;

  /*_p20.FOLE*/ logic<1> FOLE_VOL_CLK0;
  /*_p20.ETEF*/ logic<1> ETEF_VOL_CLK1;
  /*_p20.EDYF*/ logic<1> EDYF_VOL_CLK2;
  /*_p20.ELAF*/ logic<1> ELAF_VOL_CLK3;

  /*_p20.FEKO*/ logic<1> FEKO_VOL0;
  /*_p20.FATY*/ logic<1> FATY_VOL1;
  /*_p20.FERU*/ logic<1> FERU_VOL2;
  /*_p20.FYRO*/ logic<1> FYRO_VOL3;

  //----------
  // Output

  /*_p20.GEVY*/ logic<1> GEVY_CH4_AMP_ENn;
  /*_p20.AKOF*/ logic<1> AKOF_CH4_DAC0;
  /*_p20.BYZY*/ logic<1> BYZY_CH4_DAC1;
  /*_p20.APYR*/ logic<1> APYR_CH4_DAC2;
  /*_p20.BOZA*/ logic<1> BOZA_CH4_DAC3;

  //----------
  // Debug

  /*_p20.DYRY*/ logic<1> DYRY_DBG;
  /*_p20.COMO*/ logic<1> COMO_DBG;
  /*_p20.BAGU*/ logic<1> BAGU_DBG;
  /*_p20.BEFA*/ logic<1> BEFA_DBG;
};

//-----------------------------------------------------------------------------

#endif // SCH_CHANNEL4_H
