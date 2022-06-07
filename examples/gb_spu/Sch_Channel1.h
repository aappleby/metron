/// plait_noparse

#include "gates.h"
#include "regs.h"
#include "metron_tools.h"

#pragma once

//-----------------------------------------------------------------------------

struct Channel1 {

  //----------
  // FF10

  /*_p11.BANY*/ DFF9 BANY_NR10_SHIFT0;
  /*_p11.ARAX*/ DFF9 ARAX_NR10_SHIFT1;
  /*_p11.ANAZ*/ DFF9 ANAZ_NR10_SHIFT2;
  /*_p11.AVAF*/ DFF9 AVAF_NR10_SWEEP_DIR;
  /*_p11.ADEK*/ DFF9 ADEK_NR10_DELAY0;
  /*_p11.BANA*/ DFF9 BANA_NR10_DELAY1;
  /*_p11.BOTU*/ DFF9 BOTU_NR10_DELAY2;

  //----------
  // FF11

  /*_p11.CENA*/ DFF9 CENA_NR11_DUTY_0;
  /*_p11.DYCA*/ DFF9 DYCA_NR11_DUTY_1;

  //----------
  // FF12

  /*_p11.JUSA*/ DFF9 JUSA_NR12_DELAY0;
  /*_p11.JUZY*/ DFF9 JUZY_NR12_DELAY1;
  /*_p11.JOMA*/ DFF9 JOMA_NR12_DELAY2;
  /*_p11.JAFY*/ DFF9 JAFY_NR12_ENV_DIR;
  /*_p11.JATY*/ DFF9 JATY_NR12_VOL0;
  /*_p11.JAXO*/ DFF9 JAXO_NR12_VOL1;
  /*_p11.JENA*/ DFF9 JENA_NR12_VOL2;
  /*_p11.JOPU*/ DFF9 JOPU_NR12_VOL3;

  //----------
  // FF13 NR13 - ch1 freq

  /*_p11.GAXE*/ DFF20 GAXE_FREQ_CNT_00;
  /*_p11.HYFE*/ DFF20 HYFE_FREQ_CNT_01;
  /*_p11.JYTY*/ DFF20 JYTY_FREQ_CNT_02;
  /*_p11.KYNA*/ DFF20 KYNA_FREQ_CNT_03;
  /*_p11.JEMA*/ DFF20 JEMA_FREQ_CNT_04;
  /*_p11.HYKE*/ DFF20 HYKE_FREQ_CNT_05;
  /*_p11.FEVA*/ DFF20 FEVA_FREQ_CNT_06;
  /*_p11.EKOV*/ DFF20 EKOV_FREQ_CNT_07;
  /*_p11.EMUS*/ DFF20 EMUS_FREQ_CNT_08;
  /*_p11.EVAK*/ DFF20 EVAK_FREQ_CNT_09;
  /*_p11.COPU*/ DFF20 COPU_FREQ_CNT_10;

  //----------
  // FF14

  /*_p11.BOKO*/ DFF9 BOKO_NR14_STOP;

  /*_p12.HORA*/ DFF11 HORA_SUM_B00;
  /*_p12.HOPA*/ DFF11 HOPA_SUM_B01;
  /*_p12.HELE*/ DFF11 HELE_SUM_B02;
  /*_p12.JAPE*/ DFF11 JAPE_SUM_B03;
  /*_p12.JETE*/ DFF11 JETE_SUM_B04;
  /*_p12.GELE*/ DFF11 GELE_SUM_B05;
  /*_p12.EPYR*/ DFF11 EPYR_SUM_B06;
  /*_p12.EDOK*/ DFF11 EDOK_SUM_B07;
  /*_p12.DEFA*/ DFF11 DEFA_SUM_B08;
  /*_p12.ETER*/ DFF11 ETER_SUM_B09;
  /*_p12.DEVA*/ DFF11 DEVA_SUM_B10;

  /*_p12.GALO*/ DFF11 GALO_SUM_A00;
  /*_p12.JODE*/ DFF11 JODE_SUM_A01;
  /*_p12.KARE*/ DFF11 KARE_SUM_A02;
  /*_p12.JYME*/ DFF11 JYME_SUM_A03;
  /*_p12.GYME*/ DFF11 GYME_SUM_A04;
  /*_p12.FAXO*/ DFF11 FAXO_SUM_A05;
  /*_p12.EXAP*/ DFF11 EXAP_SUM_A06;
  /*_p12.DELE*/ DFF11 DELE_SUM_A07;
  /*_p12.DEXE*/ DFF11 DEXE_SUM_A08;
  /*_p12.DOFY*/ DFF11 DOFY_SUM_A09;
  /*_p12.DOLY*/ DFF11 DOLY_SUM_A10;

  /*_p12.HYKA*/ DFF22 HYKA_SUMMER00;
  /*_p12.JYKA*/ DFF22 JYKA_SUMMER01;
  /*_p12.HAVO*/ DFF22 HAVO_SUMMER02;
  /*_p12.EDUL*/ DFF22 EDUL_SUMMER03;
  /*_p12.FELY*/ DFF22 FELY_SUMMER04;
  /*_p12.HOLU*/ DFF22 HOLU_SUMMER05;
  /*_p12.HYXU*/ DFF22 HYXU_SUMMER06;
  /*_p12.HOPO*/ DFF22 HOPO_SUMMER07;
  /*_p12.DYGY*/ DFF22 DYGY_SUMMER08;
  /*_p12.EVAB*/ DFF22 EVAB_SUMMER09;
  /*_p12.AXAN*/ DFF22 AXAN_SUMMER10;

  /*_p12.FABU*/ DFF22 FABU_SHIFTER_00;
  /*_p12.JEFA*/ DFF22 JEFA_SHIFTER_01;
  /*_p12.GOGA*/ DFF22 GOGA_SHIFTER_02;
  /*_p12.JOLU*/ DFF22 JOLU_SHIFTER_03;
  /*_p12.JOTA*/ DFF22 JOTA_SHIFTER_04;
  /*_p12.FUDE*/ DFF22 FUDE_SHIFTER_05;
  /*_p12.FEDO*/ DFF22 FEDO_SHIFTER_06;
  /*_p12.EXAC*/ DFF22 EXAC_SHIFTER_07;
  /*_p12.ELUX*/ DFF22 ELUX_SHIFTER_08;
  /*_p12.AGEZ*/ DFF22 AGEZ_SHIFTER_09;
  /*_p12.BEKU*/ DFF22 BEKU_SHIFTER_10;

  /*_p09.CALO*/ DFF17 CALO_RESTART_CLK;

  /*_p13.FEKU*/ DFF17 FEKU_RESTART0;
  /*_p13.FARE*/ DFF17 FARE_RESTART1;
  /*_p13.FYTE*/ DFF17 FYTE_RESTART2;


  //----------

  /*_p13.GEXU*/ NandLatch GEXU_FREQ_GATE; // def latch, looks like arms are on vcc

  /*_p13.BYTE*/ DFF17 BYTE_SHIFT_DONE_SYNC; // not positive this is DFF17, the annotation is off

  /*_p13.BACY*/ DFF20 BACY_NR11_LEN0;
  /*_p13.CAVY*/ DFF20 CAVY_NR11_LEN1;
  /*_p13.BOVY*/ DFF20 BOVY_NR11_LEN2;
  /*_p13.CUNO*/ DFF20 CUNO_NR11_LEN3;
  /*_p13.CURA*/ DFF20 CURA_NR11_LEN4;
  /*_p13.ERAM*/ DFF20 ERAM_NR11_LEN5;

  /*_p13.CERO*/ DFF17 CERO_CH1_LEN_DONE;

  /*_p13.CUPO*/ DFF20 CUPO_SWEEP_DELAY0;
  /*_p13.CYPU*/ DFF20 CYPU_SWEEP_DELAY1;
  /*_p13.CAXY*/ DFF20 CAXY_SWEEP_DELAY2;
  /*_p13.BEXA*/ DFF17 BEXA_SWEEP_TRIGGER;
  /*_p13.COMY*/ DFF17 COMY_PHASE_CLKnb;

  /*_p13.ESUT*/ DFF17 ESUT_PHASE_xBxDxFxH;
  /*_p13.EROS*/ DFF13 EROS_PHASE_xxCDxxGH;
  /*_p13.DAPE*/ DFF13 DAPE_PHASE_xxxxEFGH;

  // Sweep shift counter
  /*_p13.COPA*/ DFF20 COPA_SHIFTER_CNT0;
  /*_p13.CAJA*/ DFF20 CAJA_SHIFTER_CNT1;
  /*_p13.BYRA*/ DFF20 BYRA_SHIFTER_CNT2;

  //----------

#if 0
  /*_p13.HESU*/ logic<1> HESU_ENV_DELTA0;  // and/or? xor? what is this? not an adder...
  /*_p13.HETO*/ logic<1> HETO_ENV_DELTA1;  // and/or? xor? what is this?
  /*_p13.HYTO*/ logic<1> HYTO_ENV_DELTA2;  // and/or? xor? what is this?
  /*_p13.JUFY*/ logic<1> JUFY_ENV_DELTA3;  // and/or? xor? what is this?
#endif

  /*_p13.HEVO*/ DFF20 HEVO_ENV0;
  /*_p13.HOKO*/ DFF20 HOKO_ENV1;
  /*_p13.HEMY*/ DFF20 HEMY_ENV2;
  /*_p13.HAFO*/ DFF20 HAFO_ENV3;
  
  /*_p13.KOZY*/ DFF17 KOZY_ENV_CLK;
  /*_p13.KYNO*/ DFF17 KYNO_ENV_STOP;

  /*_p13.JOVA*/ DFF20 JOVA_ENV_DELAY0;
  /*_p13.KENU*/ DFF20 KENU_ENV_DELAY1;
  /*_p13.KERA*/ DFF20 KERA_ENV_DELAY2;

  /*_p13.DUWO*/ DFF17 DUWO_RAW_BIT_SYNC;
  /*_p13.EZEC*/ DFF17 EZEC_START;
  /*_p13.DUPE*/ DFF9  DUPE_NR14_START;
  /*_p13.KALY*/ DFF17 KALY_CLK_64;
  /*_p09.BAZA*/ DFF17 BAZA_DBG_SWEEP_CLK;

  /*_p13.CYTO*/ NorLatch CYTO_CH1_ACTIVE;
};

//-----------------------------------------------------------------------------
