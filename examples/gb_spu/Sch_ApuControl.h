/// plait_noparse

#pragma once

#include "gates.h"
#include "regs.h"

//==============================================================================
//             CONTROL
//==============================================================================

struct SpuControl {
  /*#p15.CEMO*/ DFF17 CEMO_1MHZ;
  /*_p15.ATEP*/ DFF17 ATEP_2MHZ;

  //----------
  // clocks for the apu

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















//==============================================================================
//             CHANNEL 1
//==============================================================================

struct SpuChannel1 {

  /*#p13.HOCA*/ wire HOCA_CH1_AMP_ENn() const {
    return nor5(JAFY_NR12_ENV_DIR.qn_new(),
                JATY_NR12_VOL0.qn_new(),
                JAXO_NR12_VOL1.qn_new(),
                JENA_NR12_VOL2.qn_new(),
                JOPU_NR12_VOL3.qn_new());
  }


  /*#p13.BUGE*/ wire BUGE_SWEEP_ENn() const {
    return nand3(ANAZ_NR10_SWEEP_SHIFT2.qp_new(),
                 ARAX_NR10_SWEEP_SHIFT1.qp_new(),
                 BANY_NR10_SWEEP_SHIFT0.qp_new());
  }

  /*#p13.ADAD*/ wire ADAD_SHIFT_DONE() const { return not1(BYTE_SHIFT_DONE.qn_new()); }


  /*#p11.BANY*/ DFF9 BANY_NR10_SWEEP_SHIFT0;
  /*#p11.ARAX*/ DFF9 ARAX_NR10_SWEEP_SHIFT1;
  /*#p11.ANAZ*/ DFF9 ANAZ_NR10_SWEEP_SHIFT2;
  /*#p11.AVAF*/ DFF9 AVAF_NR10_SWEEP_DIR_p;
  /*#p11.ADEK*/ DFF9 ADEK_NR10_SWEEP_PERIOD0p;
  /*#p11.BANA*/ DFF9 BANA_NR10_SWEEP_PERIOD1p;
  /*#p11.BOTU*/ DFF9 BOTU_NR10_SWEEP_PERIOD2p;

  //----------

  /*_p11.CENA*/ DFF9  CENA_NR11_DUTY_0;
  /*_p11.DYCA*/ DFF9  DYCA_NR11_DUTY_1;
  /*_p13.BACY*/ DFF20 BACY_NR11_LEN0;
  /*_p13.CAVY*/ DFF20 CAVY_NR11_LEN1;
  /*_p13.BOVY*/ DFF20 BOVY_NR11_LEN2;
  /*_p13.CUNO*/ DFF20 CUNO_NR11_LEN3;
  /*_p13.CURA*/ DFF20 CURA_NR11_LEN4;
  /*_p13.ERAM*/ DFF20 ERAM_NR11_LEN5;

  //----------

  /*_p11.JUSA*/ DFF9 JUSA_NR12_DELAY0n;
  /*_p11.JUZY*/ DFF9 JUZY_NR12_DELAY1n;
  /*_p11.JOMA*/ DFF9 JOMA_NR12_DELAY2n;
  /*_p11.JAFY*/ DFF9 JAFY_NR12_ENV_DIR;
  /*_p11.JATY*/ DFF9 JATY_NR12_VOL0;
  /*_p11.JAXO*/ DFF9 JAXO_NR12_VOL1;
  /*_p11.JENA*/ DFF9 JENA_NR12_VOL2;
  /*_p11.JOPU*/ DFF9 JOPU_NR12_VOL3;

  //----------

  /*_p11.BOKO*/ DFF9 BOKO_NR14_LEN_EN;
  /*#p13.DUPE*/ DFF9 DUPE_NR14_TRIG;

  /*#p13.CYTO*/ NorLatch CYTO_CH1_ACTIVEp;

  /*#p13.EZEC*/ DFF17 EZEC_CH1_TRIGn;

  /*_p12.HYKA*/ DFF22 HYKA_CH1_FREQ00;
  /*_p12.JYKA*/ DFF22 JYKA_CH1_FREQ01;
  /*_p12.HAVO*/ DFF22 HAVO_CH1_FREQ02;
  /*_p12.EDUL*/ DFF22 EDUL_CH1_FREQ03;
  /*_p12.FELY*/ DFF22 FELY_CH1_FREQ04;
  /*_p12.HOLU*/ DFF22 HOLU_CH1_FREQ05;
  /*_p12.HYXU*/ DFF22 HYXU_CH1_FREQ06;
  /*_p12.HOPO*/ DFF22 HOPO_CH1_FREQ07;
  /*_p12.DYGY*/ DFF22 DYGY_CH1_FREQ08;
  /*_p12.EVAB*/ DFF22 EVAB_CH1_FREQ09;
  /*_p12.AXAN*/ DFF22 AXAN_CH1_FREQ10;

  /*_p09.CALO*/ DFF17 CALO_CLK_1M;

  /*_p13.FEKU*/ DFF17 FEKU_CH1_TRIGp;
  /*_p13.FARE*/ DFF17 FARE_CH1_TRIGp;
  /*_p13.FYTE*/ DFF17 FYTE_CH1_TRIGp;

  /*_p13.BYTE*/ DFF17 BYTE_SHIFT_DONE;

  /*_p13.CERO*/ DFF17 CERO_CH1_LEN_DONE;

  /*_p11.GAXE*/ DFF20 GAXE_CH1_FREQ_CNT_00;
  /*_p11.HYFE*/ DFF20 HYFE_CH1_FREQ_CNT_01;
  /*_p11.JYTY*/ DFF20 JYTY_CH1_FREQ_CNT_02;
  /*_p11.KYNA*/ DFF20 KYNA_CH1_FREQ_CNT_03;
  /*_p11.JEMA*/ DFF20 JEMA_CH1_FREQ_CNT_04;
  /*_p11.HYKE*/ DFF20 HYKE_CH1_FREQ_CNT_05;
  /*_p11.FEVA*/ DFF20 FEVA_CH1_FREQ_CNT_06;
  /*_p11.EKOV*/ DFF20 EKOV_CH1_FREQ_CNT_07;
  /*_p11.EMUS*/ DFF20 EMUS_CH1_FREQ_CNT_08;
  /*_p11.EVAK*/ DFF20 EVAK_CH1_FREQ_CNT_09;
  /*_p11.COPU*/ DFF20 COPU_CH1_FREQ_CNT_10;  

  /*#p09.BAZA*/ DFF17 BAZA_DBG_SWEEP_CLK;
  /*_p13.BEXA*/ DFF17 BEXA_SWEEP_TRIGGERp;
  /*_p13.CUPO*/ DFF20 CUPO_SWEEP_DELAY0p;
  /*_p13.CYPU*/ DFF20 CYPU_SWEEP_DELAY1p;
  /*_p13.CAXY*/ DFF20 CAXY_SWEEP_DELAY2p;

  /*_p12.FABU*/ DFF22 FABU_CH1_SHIFT00;
  /*_p12.JEFA*/ DFF22 JEFA_CH1_SHIFT01;
  /*_p12.GOGA*/ DFF22 GOGA_CH1_SHIFT02;
  /*_p12.JOLU*/ DFF22 JOLU_CH1_SHIFT03;
  /*_p12.JOTA*/ DFF22 JOTA_CH1_SHIFT04;
  /*_p12.FUDE*/ DFF22 FUDE_CH1_SHIFT05;
  /*_p12.FEDO*/ DFF22 FEDO_CH1_SHIFT06;
  /*_p12.EXAC*/ DFF22 EXAC_CH1_SHIFT07;
  /*_p12.ELUX*/ DFF22 ELUX_CH1_SHIFT08;
  /*_p12.AGEZ*/ DFF22 AGEZ_CH1_SHIFT09;
  /*_p12.BEKU*/ DFF22 BEKU_CH1_SHIFT10;

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

  /*_p13.FEMU*/ NandLatch FEMU_SHIFTINGn;

  /*#p13.COPA*/ DFF20 COPA_SHIFTCNT0;
  /*#p13.CAJA*/ DFF20 CAJA_SHIFTCNT1;
  /*#p13.BYRA*/ DFF20 BYRA_SHIFTCNT2;

  /*_p13.FYFO*/ NorLatch FYFO_CH1_TRIGn;
  /*_p13.COMY*/ DFF17 COMY_FREQ_RSTp;
  /*_p13.GEXU*/ NandLatch GEXU_FREQ_GATE;

  /*_p13.ESUT*/ DFF17 ESUT_PHASE_xBxDxFxH;
  /*_p13.EROS*/ DFF13 EROS_PHASE_xxCDxxGH;
  /*_p13.DAPE*/ DFF13 DAPE_PHASE_xxxxEFGH;

  /*#p13.KOZY*/ DFF17 KOZY_ENV_CLKp;
  /*#p13.KYNO*/ DFF17 KYNO_ENV_STOP;

  /*_p13.JOVA*/ DFF20 JOVA_ENV_DELAY0p;
  /*_p13.KENU*/ DFF20 KENU_ENV_DELAY1p;
  /*_p13.KERA*/ DFF20 KERA_ENV_DELAY2p;  

  /*_p13.HEVO*/ DFF20 HEVO_CH1_ENV3p;
  /*_p13.HOKO*/ DFF20 HOKO_CH1_ENV2p;
  /*_p13.HEMY*/ DFF20 HEMY_CH1_ENV1p;
  /*_p13.HAFO*/ DFF20 HAFO_CH1_ENV0p;

  /*_p13.KALY*/ DFF17 KALY_ENV_DELAY_CLK_64n;

  /*_p13.DUWO*/ DFF17 DUWO_RAW_BIT_SYNCp;
  /*#p13.KEZU*/ NorLatch KEZU_ENV_ACTIVE;
};



























//==============================================================================
//             CHANNEL 2
//==============================================================================

struct SpuChannel2 {

  /*#p15.ETUK*/ wire ETUK_CLK() const { return not1(GYKO_CLK.qp_new()); }
  /*#p15.DAVU*/ wire DAVU_CLK() const { return not1(ETUK_CLK()); }
  /*#p15.CULE*/ wire CULE_CLK() const { return not1(DAVU_CLK()); }

  /*#p15.FUTE*/ wire FUTE_CH2_AMP_ENn() const {
    return nor5(FORE_NR22_ADD.qn_new(),
                GATA_NR22_V0.qn_new(),
                GUFE_NR22_V1.qn_new(),
                GURA_NR22_V2.qn_new(),
                GAGE_NR22_V3.qn_new());
  }

  /*_p15.ERYC*/ DFF20 ERYC_NR21_L0;
  /*_p15.CERA*/ DFF20 CERA_NR21_L1;
  /*_p15.CONU*/ DFF20 CONU_NR21_L2;
  /*_p15.CAME*/ DFF20 CAME_NR21_L3;
  /*_p15.BUVA*/ DFF20 BUVA_NR21_L4;
  /*_p15.AKYD*/ DFF20 AKYD_NR21_L5;
  /*_p14.BERA*/ DFF9  BERA_NR21_D6;
  /*_p14.BAMY*/ DFF9  BAMY_NR21_D7;

  //----------
  // FF17 NR22 - channel 2 vol/env/period

  /*_p14.HYFU*/ DFF9 HYFU_NR22_P0n;
  /*_p14.HORE*/ DFF9 HORE_NR22_P1n;
  /*_p14.HAVA*/ DFF9 HAVA_NR22_P2n;
  /*_p14.FORE*/ DFF9 FORE_NR22_ADD;
  /*_p14.GATA*/ DFF9 GATA_NR22_V0;
  /*_p14.GUFE*/ DFF9 GUFE_NR22_V1;
  /*_p14.GURA*/ DFF9 GURA_NR22_V2;
  /*_p14.GAGE*/ DFF9 GAGE_NR22_V3;

  //----------

  DFF9 FOFE_NR23_FREQ_00;
  DFF9 FOVA_NR23_FREQ_01;
  DFF9 FEDY_NR23_FREQ_02;
  DFF9 FOME_NR23_FREQ_03;
  DFF9 FORA_NR23_FREQ_04;
  DFF9 GODA_NR23_FREQ_05;
  DFF9 GUMY_NR23_FREQ_06;
  DFF9 GUPU_NR23_FREQ_07;

  //----------

  DFF9 JEFU_NR24_FREQ_08;
  DFF9 JANY_NR24_FREQ_09;
  DFF9 JUPY_NR24_FREQ_10;

  /*_p15.DANE*/ NorLatch DANE_CH2_ACTIVEp;

  DFF20 JORE_CH2_ENV_DELAYp;
  DFF20 JONA_CH2_ENV_DELAYp;
  DFF20 JEVY_CH2_ENV_DELAYp;
  DFF17 JOPA_CH2_ENV_TICK;

  DFF20 DONE_CH2_FREQ_00;
  DFF20 DYNU_CH2_FREQ_01;
  DFF20 EZOF_CH2_FREQ_02;
  DFF20 CYVO_CH2_FREQ_03;
  DFF20 FUXO_CH2_FREQ_04;
  DFF20 GANO_CH2_FREQ_05;
  DFF20 GOCA_CH2_FREQ_06;
  DFF20 GANE_CH2_FREQ_07;
  DFF20 HEVY_CH2_FREQ_08;
  DFF20 HEPU_CH2_FREQ_09;
  DFF20 HERO_CH2_FREQ_10;

  DFF17 DOME_CH2_OUTp;

  DFF17 ELOX_CH2_TRIGp;
  DFF17 CAZA_CH2_TRIGp;
  DFF9 EMER_NR24_LENENp;
  DFF17 DOPE_CH2_TRIGp;

  NorLatch DALA_CH2_TRIGp;

  DFF9 ETAP_NR24_TRIGp;
  DFF17 DORY_CH2_TRIGp;
  DFF17 GYKO_CLK;
  /*#p15.CANO*/ DFF17 CANO_00;
  /*#p15.CAGY*/ DFF13 CAGY_01;
  /*#p15.DYVE*/ DFF13 DYVE_02;
  NandLatch BUTA_FREQ_GATEp;
  DFF20 FENA_CH2_VOL3;
  DFF20 FOMY_CH2_VOL2;
  DFF20 FETE_CH2_VOL1;
  DFF20 FENO_CH2_VOL0;
  DFF17 HEPO;
  NorLatch JEME;

  DFF17 JYNA_CLK_64n;
  /*_p15.CYRE*/ DFF17 CYRE_CH2_LEN_DONEp;
};


























//==============================================================================
//             CHANNEL 3
//==============================================================================

struct SpuChannel3 {
  /*_p16.KOGA*/ DFF9 KOGA_NR33_FREQ00;
  /*_p16.JOVY*/ DFF9 JOVY_NR33_FREQ01;
  /*_p16.JAXA*/ DFF9 JAXA_NR33_FREQ02;
  /*_p16.JEFE*/ DFF9 JEFE_NR33_FREQ03;
  /*_p16.JYPO*/ DFF9 JYPO_NR33_FREQ04;
  /*_p16.JOVE*/ DFF9 JOVE_NR33_FREQ05;
  /*_p16.KANA*/ DFF9 KANA_NR33_FREQ06;
  /*_p16.KOGU*/ DFF9 KOGU_NR33_FREQ07;

  //----------

  /*_p16.JEMO*/ DFF9 JEMO_NR34_FREQ08;
  /*_p16.JETY*/ DFF9 JETY_NR34_FREQ09;
  /*_p16.JACY*/ DFF9 JACY_NR34_FREQ10;
  /*_p16.HOTO*/ DFF9 HOTO_NR34_LENEN;
  /*_p16.GAVU*/ DFF9 GAVU_NR34_TRIG;

  /*_p18.DAVO*/ DFF17    DAVO_CH3_ACTIVEp;

  /*_p16.GOFY*/ NorLatch GOFY_CH3_ACTIVEn;
  NorLatch FOZU;


  /*_p16.GUXE*/ DFF9 GUXE_CH3_AMP_ENn;

  /*_p18.GEVO*/ DFF20 GEVO_CH3_LEN0p;
  /*_p18.FORY*/ DFF20 FORY_CH3_LEN1p;
  /*_p18.GATU*/ DFF20 GATU_CH3_LEN2p;
  /*_p18.GAPO*/ DFF20 GAPO_CH3_LEN3p;
  /*_p18.GEMO*/ DFF20 GEMO_CH3_LEN4p;
  /*_p18.FORO*/ DFF20 FORO_CH3_LEN5p;
  /*_p18.FAVE*/ DFF20 FAVE_CH3_LEN6p;
  /*_p18.FYRU*/ DFF20 FYRU_CH3_LEN7p;  

  /*_p18.FEXU*/ DFF17 FEXU_LEN_DONEp;

  /*_p16.HUKY*/ DFF9 HUKY_NR32_VOL0;
  /*_p16.HODY*/ DFF9 HODY_NR32_VOL1;

  /*_p16.GARA*/ DFF17 GARA;
  /*_p16.GYTA*/ DFF17 GYTA;
  /*_p16.GYRA*/ DFF17 GYRA;

  /*_p18.KUTU*/ DFF20 KUTU_FREQ_00;
  /*_p18.KUPE*/ DFF20 KUPE_FREQ_01;
  /*_p18.KUNU*/ DFF20 KUNU_FREQ_02;
  /*_p18.KEMU*/ DFF20 KEMU_FREQ_03;
  /*_p18.KYGU*/ DFF20 KYGU_FREQ_04;
  /*_p18.KEPA*/ DFF20 KEPA_FREQ_05;
  /*_p18.KAFO*/ DFF20 KAFO_FREQ_06;
  /*_p18.KENO*/ DFF20 KENO_FREQ_07;
  /*_p18.KEJU*/ DFF20 KEJU_FREQ_08;
  /*_p18.KEZA*/ DFF20 KEZA_FREQ_09;
  /*_p18.JAPU*/ DFF20 JAPU_FREQ_10;

  /*_p18.HUNO*/ DFF17 HUNO_WAVE_CLKo;
  /*_p18.EFAR*/ DFF17 EFAR_WAVE_IDX0;
  /*_p18.ERUS*/ DFF17 ERUS_WAVE_IDX1;
  /*_p18.EFUZ*/ DFF17 EFUZ_WAVE_IDX2;
  /*_p18.EXEL*/ DFF17 EXEL_WAVE_IDX3;
  /*_p18.EFAL*/ DFF17 EFAL_WAVE_IDX4;
  /*_p18.FETY*/ DFF17 FETY_WAVE_LOOP;

  /*_p17.CYFO*/ DFF9 CYFO_SAMPLE_0;
  /*_p17.CESY*/ DFF9 CESY_SAMPLE_1;
  /*_p17.BUDY*/ DFF9 BUDY_SAMPLE_2;
  /*_p17.BEGU*/ DFF9 BEGU_SAMPLE_3;
  /*_p17.CUVO*/ DFF9 CUVO_SAMPLE_4;
  /*_p17.CEVO*/ DFF9 CEVO_SAMPLE_5;
  /*_p17.BORA*/ DFF9 BORA_SAMPLE_6;
  /*_p17.BEPA*/ DFF9 BEPA_SAMPLE_7;

  /*_p16.FOBA*/ DFF17 FOBA_CH3_TRIGp;
  NandLatch GUGU_FREQ_CLK_STOP;

  // This is a pulse generator for wave ram control line 2/3
  /*_p17.BUSA*/ DFF17 BUSA_WAVE_CLK_D1;
  /*_p17.BANO*/ DFF17 BANO_WAVE_CLK_D2;
  /*_p17.AZUS*/ DFF17 AZUS_WAVE_CLK_D3;
  /*_p17.AZET*/ DFF17 AZET_WAVE_CLK_D4;
};





























//==============================================================================
//             CHANNEL 4
//==============================================================================

struct SpuChannel4 {
  /*_p19.EMOK*/ DFF9 EMOK_NR42_ENV_TIMER0;
  /*_p19.ETYJ*/ DFF9 ETYJ_NR42_ENV_TIMER1;
  /*_p19.EZYK*/ DFF9 EZYK_NR42_ENV_TIMER2;
  /*_p19.GEKY*/ DFF9 GEKY_NR42_ENV_DIR;
  /*_p19.GARU*/ DFF9 GARU_NR42_ENV0;
  /*_p19.GOKY*/ DFF9 GOKY_NR42_ENV1;
  /*_p19.GOZO*/ DFF9 GOZO_NR42_ENV2;
  /*_p19.GEDU*/ DFF9 GEDU_NR42_ENV3;

  //----------

  /*_p19.JARE*/ DFF9 JARE_NR43_DIV0;
  /*_p19.JERO*/ DFF9 JERO_NR43_DIV1;
  /*_p19.JAKY*/ DFF9 JAKY_NR43_DIV2;
  /*_p19.JAMY*/ DFF9 JAMY_NR43_MODE;
  /*_p19.FETA*/ DFF9 FETA_NR43_FREQ0;
  /*_p19.FYTO*/ DFF9 FYTO_NR43_FREQ1;
  /*_p19.GOGO*/ DFF9 GOGO_NR43_FREQ2;
  /*_p19.GAFO*/ DFF9 GAFO_NR43_FREQ3;

  /*_p20.GENA*/ NorLatch GENA_CH4_ACTIVEp;

  //========================================
  //             CHANNEL 4
  //========================================

  /*_p20.GONE*/ DFF17 GONE_CH4_TRIGp;
  /*_p20.GORA*/ DFF17 GORA_CH4_TRIGp;
  /*_p20.GATY*/ DFF17 GATY_CH4_TRIGp;
  /*_p19.FUGO*/ DFF17 FUGO_CH4_LEN_DONEp;

  /*#p19.DANO*/ DFF20 DANO_NR41_LEN0;
  /*#p19.FAVY*/ DFF20 FAVY_NR41_LEN1;
  /*#p19.DENA*/ DFF20 DENA_NR41_LEN2;
  /*#p19.CEDO*/ DFF20 CEDO_NR41_LEN3;
  /*#p19.FYLO*/ DFF20 FYLO_NR41_LEN4;
  /*_p19.EDOP*/ DFF20 EDOP_NR41_LEN5;

  /*#p19.CUNY*/ DFF9 CUNY_NR44_LEN_ENp;
  /*#p19.HOGA*/ DFF9 HOGA_NR44_TRIG;
  /*#p20.GYSU*/ DFF17 GYSU_CH4_TRIG;

  /*#p20.HAZO*/ NorLatch HAZO_CH4_TRIGn;
  /*#p20.JERY*/ NandLatch JERY_DIV_GATE;
  /*#p20.JYCO*/ DFF20 JYCO_DIV0;
  /*#p20.JYRE*/ DFF20 JYRE_DIV1;
  /*#p20.JYFU*/ DFF20 JYFU_DIV2;
  /*#p20.GARY*/ DFF17 GARY_FREQ_GATEp;

  /*_p20.CEXO*/ DFF17 CEXO_FREQ_00;
  /*_p20.DEKO*/ DFF17 DEKO_FREQ_01;
  /*_p20.EZEF*/ DFF17 EZEF_FREQ_02;
  /*_p20.EPOR*/ DFF17 EPOR_FREQ_03;
  /*_p20.DURE*/ DFF17 DURE_FREQ_04;
  /*_p20.DALE*/ DFF17 DALE_FREQ_05;
  /*_p20.DOKE*/ DFF17 DOKE_FREQ_06;
  /*_p20.DEMO*/ DFF17 DEMO_FREQ_07;
  /*_p20.DOSE*/ DFF17 DOSE_FREQ_08;
  /*_p20.DETE*/ DFF17 DETE_FREQ_09;
  /*_p20.ERUT*/ DFF17 ERUT_FREQ_10;
  /*_p20.DOTA*/ DFF17 DOTA_FREQ_11;
  /*_p20.DERE*/ DFF17 DERE_FREQ_12;
  /*_p20.ESEP*/ DFF17 ESEP_FREQ_13;

  /*_p20.JOTO*/ DFF17 JOTO_LFSR_00;
  /*_p20.KOMU*/ DFF17 KOMU_LFSR_01;
  /*_p20.KETU*/ DFF17 KETU_LFSR_02;
  /*_p20.KUTA*/ DFF17 KUTA_LFSR_03;
  /*_p20.KUZY*/ DFF17 KUZY_LFSR_04;
  /*_p20.KYWY*/ DFF17 KYWY_LFSR_05;
  /*_p20.JAJU*/ DFF17 JAJU_LFSR_06;
  /*_p20.HAPE*/ DFF17 HAPE_LFSR_07;
  /*_p20.JUXE*/ DFF17 JUXE_LFSR_08;
  /*_p20.JEPE*/ DFF17 JEPE_LFSR_09;
  /*_p20.JAVO*/ DFF17 JAVO_LFSR_10;
  /*_p20.HEPA*/ DFF17 HEPA_LFSR_11;
  /*_p20.HORY*/ DFF17 HORY_LFSR_12;
  /*_p20.HENO*/ DFF17 HENO_LFSR_13;
  /*_p20.HYRO*/ DFF17 HYRO_LFSR_14;
  /*_p20.HEZU*/ DFF17 HEZU_LFSR_15;

  /*#p20.ABEL*/ DFF17 ABEL_CLK_64;
  /*#p20.CUNA*/ DFF20 CUNA_ENV_DELAY0n;
  /*#p20.COFE*/ DFF20 COFE_ENV_DELAY1n;
  /*#p20.DOGO*/ DFF20 DOGO_ENV_DELAY2n;

  /*#p20.FOSY*/ DFF17 FOSY_ENV_CLKp;
  /*#p20.FYNO*/ DFF17 FYNO_ENV_MAXp;

  /*#p20.FEKO*/ DFF20 FEKO_CH4_VOL0;
  /*#p20.FATY*/ DFF20 FATY_CH4_VOL1;
  /*#p20.FERU*/ DFF20 FERU_CH4_VOL2;
  /*#p20.FYRO*/ DFF20 FYRO_CH4_VOL3;
  /*#p20.EROX*/ NorLatch EROX_ENV_RUNNINGn;
};



























//==============================================================================
//             TOP
//==============================================================================

struct GBSound {
  wire SYKE_ADDR_FFXX = 0;
  wire UNOR_MODE_DBG2 = 0;
  wire ALUR_SYS_RSTn = true;
  wire TEDO_CPU_RDp = false;
  wire TAPU_CPU_WRp = false;
  wire AZOF_xBxDxFxH = 0;
  wire BUDE_xxxxEFGH = 0;
  wire UMER_DIV10n_old = 0;
  wire SIG_CPU_HOLD_MEM = 0;
  wire SIG_IN_CPU_DBUS_FREE = 0;

  void tick();

  void tick_nr10();
  void tick_nr11();
  void tick_nr12();
  void tick_nr13();
  void tick_nr14();

  void tick_nr21();
  void tick_nr22();
  void tick_nr23();
  void tick_nr24();

  void tick_nr30();
  void tick_nr31();
  void tick_nr32();
  void tick_nr33();
  void tick_nr34();

  void tick_nr41();
  void tick_nr42();
  void tick_nr43();
  void tick_nr44();

  void tick_nr50();
  void tick_nr51();
  void tick_nr52();

  //----------
  // CPU read/write signals

  /*_p09.AGUZ*/ wire AGUZ_CPU_RDn() const { return not1(TEDO_CPU_RDp); }
  /*#p19.BYLO*/ wire BYLO_CPU_RDp() const { return not1(AGUZ_CPU_RDn()); }
  /*_p09.GAXO*/ wire GAXO_CPU_RDp() const { return not1(AGUZ_CPU_RDn()); }
  /*#p20.COSA*/ wire COSA_CPU_RDp() const { return not1(AGUZ_CPU_RDn()); }
  /*_p14.FOGE*/ wire FOGE_CPU_RDp() const { return not1(AGUZ_CPU_RDn()); }
  /*_p16.DOVO*/ wire DOVO_CPU_RDp() const { return not1(AGUZ_CPU_RDn()); }
  /*#p11.CEGE*/ wire CEGE_CPU_RDp() const { return not1(AGUZ_CPU_RDn()); }
  /*#pXX.GADO*/ wire GADO_CPU_RDp() const { return not1(AGUZ_CPU_RDn()); }

  /*_p10.BAFU*/ wire BAFU_CPU_WRn() const { return not1(TAPU_CPU_WRp); }
  /*_p10.BOGY*/ wire BOGY_CPU_WRp() const { return not1(BAFU_CPU_WRn()); }

  //----------
  // Debug control signals

  /*_p16.ANUJ*/ wire ANUJ_CPU_WR_WEIRD()  const { return and2(SIG_IN_CPU_DBUS_FREE, BOGY_CPU_WRp()); }
  /*_p09.EDEK*/ wire EDEK_NR52_DBG_APUp() const { return not1(spu.FERO_NR52_DBG_APUn.qn_new()); }
  /*_p14.FAPE*/ wire FAPE_CPU_RDp_DBGp()  const { return and2(FOGE_CPU_RDp(), EDEK_NR52_DBG_APUp()); }
  /*_p16.EGAD*/ wire EGAD_CPU_RDn_DBGn()  const { return nand2(DOVO_CPU_RDp(), EDEK_NR52_DBG_APUp()); }
  /*#p11.DAXA*/ wire DAXA_CPU_RDn_DBGn()  const { return nand2(CEGE_CPU_RDp(), EDEK_NR52_DBG_APUp()); }

  //----------
  // SPU clock tree

  /*_p01.ATAG*/ wire ATAG_AxCxExGx() const { return not1(AZOF_xBxDxFxH); }
  /*_p01.DOVA*/ wire DOVA_ABCDxxxx() const { return not1(BUDE_xxxxEFGH); } // this goes to all the trigger regs

  /*_p01.AMUK*/ wire AMUK_xBxDxFxH() const { return not1(ATAG_AxCxExGx()); } // goes to clock generators and wave ram clock
  /*_p01.CYBO*/ wire CYBO_AxCxExGx() const { return not1(AMUK_xBxDxFxH()); }
  /*_p01.ARYF*/ wire ARYF_AxCxExGx() const { return not1(AMUK_xBxDxFxH()); }
  /*_p01.APUV*/ wire APUV_AxCxExGx() const { return not1(AMUK_xBxDxFxH()); }
  /*_p15.AZEG*/ wire AZEG()          const { return not1(AMUK_xBxDxFxH()); }

  //----------
  // SPU reset tree

  /*_p09.HAPO*/ wire HAPO_SYS_RSTp() const { return not1(ALUR_SYS_RSTn); }
  /*_p09.GUFO*/ wire GUFO_SYS_RSTn() const { return not1(HAPO_SYS_RSTp()); }

  /*_p09.JYRO*/ wire JYRO_APU_RSTp() const { return  or2(HAPO_SYS_RSTp(), spu.HADA_NR52_ALL_SOUND_ON.qn_new()); }
  /*_p09.KEPY*/ wire KEPY_APU_RSTn() const { return not1(JYRO_APU_RSTp()); }
  /*_p09.KUBY*/ wire KUBY_APU_RSTn() const { return not1(JYRO_APU_RSTp()); }
  /*_p09.KEBA*/ wire KEBA_APU_RSTp() const { return not1(KUBY_APU_RSTn()); }

  /*_p01.ATUS*/ wire ATUS_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p01.BELA*/ wire BELA_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p01.BOPO*/ wire BOPO_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p09.ATYV*/ wire ATYV_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p09.KAME*/ wire KAME_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p14.HUDE*/ wire HUDE_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p14.JYBU*/ wire JYBU_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p15.BUWE*/ wire BUWE_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p15.CEXE*/ wire CEXE_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p15.CYWU*/ wire CYWU_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*#p09.AFAT*/ wire AFAT_APU_RSTn() const { return not1(KEBA_APU_RSTp()); } // ch2
  /*#p09.AGUR*/ wire AGUR_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*#p11.CAMY*/ wire CAMY_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*#p11.CEPO*/ wire CEPO_APU_RSTn() const { return not1(KEBA_APU_RSTp()); } // ch1
  /*#p19.CABE*/ wire CABE_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p14.KYPU*/ wire KYPU_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p14.FAZO*/ wire FAZO_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }
  /*_p16.GOVE*/ wire GOVE_APU_RSTn() const { return not1(KEBA_APU_RSTp()); }

  //----------

  /*_p10.DYTE*/ wire DYTE_A00n() const { return not1(BUS_CPU_A00p.out_new()); }
  /*_p10.AFOB*/ wire AFOB_A01n() const { return not1(BUS_CPU_A01p.out_new()); }
  /*_p10.ABUB*/ wire ABUB_A02n() const { return not1(BUS_CPU_A02p.out_new()); }
  /*_p10.ACOL*/ wire ACOL_A03n() const { return not1(BUS_CPU_A03p.out_new()); }
  /*#p10.ATUP*/ wire ATUP_A04n() const { return not1(BUS_CPU_A04p.out_new()); }
  /*#p10.BOXY*/ wire BOXY_A05n() const { return not1(BUS_CPU_A05p.out_new()); }

  /*_p10.DOSO*/ wire DOSO_A00p() const { return not1(DYTE_A00n()); }
  /*_p10.DUPA*/ wire DUPA_A01p() const { return not1(AFOB_A01n()); }
  /*_p10.DENO*/ wire DENO_A02p() const { return not1(ABUB_A02n()); }
  /*_p10.DUCE*/ wire DUCE_A03p() const { return not1(ACOL_A03n()); }

  /*_p10.ATEG*/ wire ATEG_ADDR_XX1Xn() const { return or4(BUS_CPU_A07p.out_new(), BUS_CPU_A06p.out_new(), BUS_CPU_A05p.out_new(), ATUP_A04n()); }
  /*_p10.AWET*/ wire AWET_ADDR_XX2Xn() const { return or4(BUS_CPU_A07p.out_new(), BUS_CPU_A06p.out_new(), BOXY_A05n(), BUS_CPU_A04p.out_new()); }

  /*_p10.BUNO*/ wire BUNO_ADDR_FF1Xp() const { return nor2(BAKO_ADDR_FFXXn(), ATEG_ADDR_XX1Xn()); }
  /*_p10.BANU*/ wire BANU_ADDR_FF1Xn() const { return not1(BUNO_ADDR_FF1Xp()); }

  /*_p07.BAKO*/ wire BAKO_ADDR_FFXXn() const { return not1(SYKE_ADDR_FFXX); }
  /*_p10.BEZY*/ wire BEZY_ADDR_FF2Xn() const { return  or2(AWET_ADDR_XX2Xn(), BAKO_ADDR_FFXXn()); }
  /*_p10.CONA*/ wire CONA_ADDR_FF2Xp() const { return not1(BEZY_ADDR_FF2Xn()); }

  //----------

  /*#p10.DUPO*/ wire DUPO_ADDR_0000n() const { return nand4(ACOL_A03n(), ABUB_A02n(), AFOB_A01n(), DYTE_A00n()); }
  /*#p10.DUNO*/ wire DUNO_ADDR_0001n() const { return nand4(ACOL_A03n(), ABUB_A02n(), AFOB_A01n(), DOSO_A00p()); }
  /*#p10.DAMY*/ wire DAMY_ADDR_0010n() const { return nand4(ACOL_A03n(), ABUB_A02n(), DUPA_A01p(), DYTE_A00n()); }
  /*#p10.ETUF*/ wire ETUF_ADDR_0011n() const { return nand4(ACOL_A03n(), ABUB_A02n(), DUPA_A01p(), DOSO_A00p()); }
  /*#p10.DUFE*/ wire DUFE_ADDR_0011n() const { return nand4(DOSO_A00p(), DUPA_A01p(), ABUB_A02n(), ACOL_A03n()); }
  /*_p10.DATU*/ wire DATU_ADDR_0100n() const { return nand4(ACOL_A03n(), DENO_A02p(), AFOB_A01n(), DYTE_A00n()); }
  /*#p10.ESOT*/ wire ESOT_ADDR_0100n() const { return nand4(ACOL_A03n(), DENO_A02p(), AFOB_A01n(), DYTE_A00n()); }
  /*_p10.DURA*/ wire DURA_ADDR_0101n() const { return nand4(ACOL_A03n(), DENO_A02p(), AFOB_A01n(), DOSO_A00p()); }
  /*_p10.DAZA*/ wire DAZA_ADDR_0110n() const { return nand4(ACOL_A03n(), DENO_A02p(), DUPA_A01p(), DYTE_A00n()); }
  /*_p10.EKAG*/ wire EKAG_ADDR_0110n() const { return nand4(ACOL_A03n(), DENO_A02p(), DUPA_A01p(), DYTE_A00n()); }
  /*_p10.DUVU*/ wire DUVU_ADDR_0111n() const { return nand4(ACOL_A03n(), DENO_A02p(), DUPA_A01p(), DOSO_A00p()); }
  /*_p10.DAFY*/ wire DAFY_ADDR_1000n() const { return nand4(DUCE_A03p(), ABUB_A02n(), AFOB_A01n(), DYTE_A00n()); }
  /*_p10.DEJY*/ wire DEJY_ADDR_1001n() const { return nand4(DUCE_A03p(), ABUB_A02n(), AFOB_A01n(), DOSO_A00p()); }
  /*_p10.EXAT*/ wire EXAT_ADDR_1010n() const { return nand4(DUCE_A03p(), ABUB_A02n(), DUPA_A01p(), DYTE_A00n()); }

  /*#p10.DYVA*/ wire DYVA_ADDR_FF10p() const { return nor2(BANU_ADDR_FF1Xn(), DUPO_ADDR_0000n()); }
  /*#p10.CAXE*/ wire CAXE_ADDR_FF11p() const { return nor2(BANU_ADDR_FF1Xn(), DUNO_ADDR_0001n()); }
  /*#p10.EDAF*/ wire EDAF_ADDR_FF12p() const { return nor2(BANU_ADDR_FF1Xn(), DAMY_ADDR_0010n()); }
  /*#p10.DECO*/ wire DECO_ADDR_FF13p() const { return nor2(BANU_ADDR_FF1Xn(), ETUF_ADDR_0011n()); }
  /*#p10.DUJA*/ wire DUJA_ADDR_FF14p() const { return nor2(BANU_ADDR_FF1Xn(), ESOT_ADDR_0100n()); }

  /*_p10.COVY*/ wire COVY_ADDR_FF16p() const { return nor2(BANU_ADDR_FF1Xn(), DAZA_ADDR_0110n()); }
  /*_p10.DUTU*/ wire DUTU_ADDR_FF17p() const { return nor2(BANU_ADDR_FF1Xn(), DUVU_ADDR_0111n()); }
  /*_p10.DARA*/ wire DARA_ADDR_FF18p() const { return nor2(BANU_ADDR_FF1Xn(), DAFY_ADDR_1000n()); }
  /*_p10.DOZA*/ wire DOZA_ADDR_FF19p() const { return nor2(BANU_ADDR_FF1Xn(), DEJY_ADDR_1001n()); }
  /*_p10.EMOR*/ wire EMOR_ADDR_FF1Ap() const { return nor2(BANU_ADDR_FF1Xn(), EXAT_ADDR_1010n()); }

  /*#p10.CUGE*/ wire CUGE_ADDR_FF23p() const { return nor2(DUFE_ADDR_0011n(), BEZY_ADDR_FF2Xn()); }
  /*_p10.CAFY*/ wire CAFY_ADDR_FF24p() const { return nor2(BEZY_ADDR_FF2Xn(), DATU_ADDR_0100n()); }
  /*_p10.CORA*/ wire CORA_ADDR_FF25p() const { return nor2(BEZY_ADDR_FF2Xn(), DURA_ADDR_0101n()); }

  /*#p11.BUZE*/ wire BUZE_ADDR_FF10n() const { return not1(DYVA_ADDR_FF10p()); }
  /*#p11.HAXE*/ wire HAXE_ADDR_FF12n() const { return not1(EDAF_ADDR_FF12p()); }
  /*#p11.GAGO*/ wire GAGO_ADDR_FF12n() const { return not1(EDAF_ADDR_FF12p()); }
  /*#p11.CACA*/ wire CACA_ADDR_FF13n() const { return not1(DECO_ADDR_FF13p()); }
  /*#p11.CURE*/ wire CURE_ADDR_FF14n() const { return not1(DUJA_ADDR_FF14p()); }
  /*_p09.BYMA*/ wire BYMA_ADDR_FF24n() const { return not1(CAFY_ADDR_FF24p()); }
  /*_p09.GEPA*/ wire GEPA_ADDR_FF25n() const { return not1(CORA_ADDR_FF25p()); }




  /*#p01.BATA*/ wire BATA_CLK_2M() const { return not1(spu.AJER_2M.qp_new()); }
  /*_p01.BAVU*/ wire BAVU_1M() const { return not1(spu.AVOK_1M.qp_new()); }





  /*#p01.HAMA*/ wire HAMA_CLK_512K() const { return not1(spu.JESO_CLK_512K.qp_new()); }
  /*_p01.BURE*/ wire BURE_CLK_512() const { return not1(spu.BARA_CLK_512.qp_new()); }

  /*_p01.HORU*/ wire HORU_CLK_512p() const {
    /*_p01.FYNE*/ wire FYNE_CLK_512 = not1(BURE_CLK_512());
    /*_p01.GALE*/ wire GALE_CLK_512 = mux2p(spu.FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K(), FYNE_CLK_512);
    /*_p01.GEXY*/ wire GEXY_CLK_512 = not1(GALE_CLK_512);
    /*_p01.HORU*/ wire HORU_CLK_512p = not1(GEXY_CLK_512);
    return HORU_CLK_512p;
  }

  /*_p01.BUFY*/ wire BUFY_CLK_256n() const {
    /*_p01.CULO*/ wire CULO_CLK_256 = not1(spu.CARU_CLK_256.qp_new());
    /*_p01.BEZE*/ wire BEZE_CLK_256 = mux2p(spu.FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K(), CULO_CLK_256);
    /*_p01.COFU*/ wire COFU_CLK_256 = not1(BEZE_CLK_256);
    /*_p01.BUFY*/ wire BUFY_CLK_256n = not1(COFU_CLK_256);
    return BUFY_CLK_256n;
  }

  /*_p01.BYFE*/ wire BYFE_CLK_128n() const {
    /*_p01.APEF*/ wire APEF_CLK_128 = not1(spu.BYLU_CLK_128.qp_new());
    /*_p01.BULE*/ wire BULE_CLK_128 = mux2p(spu.FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K(), APEF_CLK_128);
    /*_p01.BARU*/ wire BARU_CLK_128 = not1(BULE_CLK_128);
    /*_p01.BYFE*/ wire BYFE_CLK_128n = not1(BARU_CLK_128);
    return BYFE_CLK_128n;
  }





  SpuControl  spu;
  SpuChannel1 ch1;
  SpuChannel2 ch2;
  SpuChannel3 ch3;
  SpuChannel4 ch4;

  /*_BUS_CPU_A00p*/ Bus BUS_CPU_A00p;
  /*_BUS_CPU_A01p*/ Bus BUS_CPU_A01p;
  /*_BUS_CPU_A02p*/ Bus BUS_CPU_A02p;
  /*_BUS_CPU_A03p*/ Bus BUS_CPU_A03p;
  /*_BUS_CPU_A04p*/ Bus BUS_CPU_A04p;
  /*_BUS_CPU_A05p*/ Bus BUS_CPU_A05p;
  /*_BUS_CPU_A06p*/ Bus BUS_CPU_A06p;
  /*_BUS_CPU_A07p*/ Bus BUS_CPU_A07p;
  /*_BUS_CPU_A08p*/ Bus BUS_CPU_A08p;
  /*_BUS_CPU_A09p*/ Bus BUS_CPU_A09p;
  /*_BUS_CPU_A10p*/ Bus BUS_CPU_A10p;
  /*_BUS_CPU_A11p*/ Bus BUS_CPU_A11p;
  /*_BUS_CPU_A12p*/ Bus BUS_CPU_A12p;
  /*_BUS_CPU_A13p*/ Bus BUS_CPU_A13p;
  /*_BUS_CPU_A14p*/ Bus BUS_CPU_A14p;
  /*_BUS_CPU_A15p*/ Bus BUS_CPU_A15p;

  /*_BUS_CPU_D00p*/ Bus BUS_CPU_D00p;
  /*_BUS_CPU_D01p*/ Bus BUS_CPU_D01p;
  /*_BUS_CPU_D02p*/ Bus BUS_CPU_D02p;
  /*_BUS_CPU_D03p*/ Bus BUS_CPU_D03p;
  /*_BUS_CPU_D04p*/ Bus BUS_CPU_D04p;
  /*_BUS_CPU_D05p*/ Bus BUS_CPU_D05p;
  /*_BUS_CPU_D06p*/ Bus BUS_CPU_D06p;
  /*_BUS_CPU_D07p*/ Bus BUS_CPU_D07p;

  /*_BUS_WAVE_D00p*/ Bus BUS_WAVE_D00p;
  /*_BUS_WAVE_D01p*/ Bus BUS_WAVE_D01p;
  /*_BUS_WAVE_D02p*/ Bus BUS_WAVE_D02p;
  /*_BUS_WAVE_D03p*/ Bus BUS_WAVE_D03p;
  /*_BUS_WAVE_D04p*/ Bus BUS_WAVE_D04p;
  /*_BUS_WAVE_D05p*/ Bus BUS_WAVE_D05p;
  /*_BUS_WAVE_D06p*/ Bus BUS_WAVE_D06p;
  /*_BUS_WAVE_D07p*/ Bus BUS_WAVE_D07p;
};
