/// plait_noparse


#include "Sch_ApuControl.h"

//-----------------------------------------------------------------------------

void GBSound::tick_nr11() {

  /*#p11.BUWA*/ wire BUWA_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*#p11.BEXU*/ wire BEXU_NR11_RDn = nand2(BUWA_CPU_RDp, CAXE_ADDR_FF11p());
  /*#p11.COVU*/ wire COVU_NR11_WR = and2(BOGY_CPU_WRp(), CAXE_ADDR_FF11p());

  /*#p11.DAFO*/ wire DAFO_NR11_CLK = not1(COVU_NR11_WR);
  /*#p11.CENA*/ ch1.CENA_NR11_DUTY_0.dff9(DAFO_NR11_CLK, CEPO_APU_RSTn(), BUS_CPU_D06p.qp_new());
  /*#p11.DYCA*/ ch1.DYCA_NR11_DUTY_1.dff9(DAFO_NR11_CLK, CEPO_APU_RSTn(), BUS_CPU_D07p.qp_new());

  /*#p13.CAPY*/ wire CAPY_CH1_LEN_CLKn = nor3(ch1.CERO_CH1_LEN_DONE.qp_new(), BUFY_CLK_256n(), ch1.BOKO_NR14_LEN_EN.qp_new());
  /*#p13.CANU*/ wire CANU_CH1_LEN_CLK = not1(CAPY_CH1_LEN_CLKn);

  /*#p13.BORO*/ wire BORO_FF11_WRna = nand2(BOGY_CPU_WRp(), CAXE_ADDR_FF11p());
  /*#p13.BEPE*/ wire BEPE_FF11_WRa = not1(BORO_FF11_WRna);
  /*#p13.BUGY*/ wire BUGY_FF11_WRc = not1(BORO_FF11_WRna);
  /*#p13.BOKA*/ wire BOKA_FF11_WRb = not1(BORO_FF11_WRna);

  /*#p13.CORY*/ wire CORY_CH1_LEN_DONE_RST = nor3(BOKA_FF11_WRb, KEBA_APU_RSTp(), ch1.FEKU_CH1_TRIGp.qp_new());
  
  /*_p13.CERO*/ ch1.CERO_CH1_LEN_DONE.dff17(ch1.ERAM_NR11_LEN5.qn_new(), CORY_CH1_LEN_DONE_RST, ch1.CERO_CH1_LEN_DONE.qn_old());

  /*#p13.BACY*/ ch1.BACY_NR11_LEN0.dff20(CANU_CH1_LEN_CLK,              BUGY_FF11_WRc, BUS_CPU_D00p.qp_new());
  /*#p13.CAVY*/ ch1.CAVY_NR11_LEN1.dff20(ch1.BACY_NR11_LEN0.qp_new(),   BUGY_FF11_WRc, BUS_CPU_D01p.qp_new());
  /*#p13.BOVY*/ ch1.BOVY_NR11_LEN2.dff20(ch1.CAVY_NR11_LEN1.qp_new(),   BUGY_FF11_WRc, BUS_CPU_D02p.qp_new());
  /*#p13.CUNO*/ ch1.CUNO_NR11_LEN3.dff20(ch1.BOVY_NR11_LEN2.qp_new(),   BUGY_FF11_WRc, BUS_CPU_D03p.qp_new());

  /*#p13.CUSO*/ wire CUSO_NR11_LEN3p = not1(ch1.CUNO_NR11_LEN3.qn_new());
  /*#p13.CURA*/ ch1.CURA_NR11_LEN4.dff20(CUSO_NR11_LEN3p,               BEPE_FF11_WRa, BUS_CPU_D04p.qp_new());
  /*#p13.ERAM*/ ch1.ERAM_NR11_LEN5.dff20(ch1.CURA_NR11_LEN4.qp_new(),   BEPE_FF11_WRa, BUS_CPU_D05p.qp_new());

  /*#p11.BOWO*/ triwire BOWO = tri6_nn(BEXU_NR11_RDn, ch1.CENA_NR11_DUTY_0.qp_new());
  /*#p11.CUDA*/ triwire CUDA = tri6_nn(BEXU_NR11_RDn, ch1.DYCA_NR11_DUTY_1.qp_new());
}

//-----------------------------------------------------------------------------

void GBSound::tick_nr12() {

  /*#p11.HAFU*/ wire HAFU_NR12_WRp = and2(BOGY_CPU_WRp(), EDAF_ADDR_FF12p());
  /*#p11.KYGY*/ wire KYGY_NR12_WRn = not1(HAFU_NR12_WRp);
  /*#p11.GAXU*/ wire GAXU_NR12_WRn = nand2(EDAF_ADDR_FF12p(), BOGY_CPU_WRp());
  wire KAGY_NR12_WRp = not1(GAXU_NR12_WRn); // not on schematic?

  /*#p11.HATO*/ wire HATO_RSTn = not1(KEBA_APU_RSTp());

  /*#p11.JUSA*/ ch1.JUSA_NR12_DELAY0n  .dff9(KYGY_NR12_WRn, HATO_RSTn, BUS_CPU_D00p.qp_new());
  /*#p11.JUZY*/ ch1.JUZY_NR12_DELAY1n  .dff9(KYGY_NR12_WRn, HATO_RSTn, BUS_CPU_D01p.qp_new());
  /*#p11.JOMA*/ ch1.JOMA_NR12_DELAY2n  .dff9(KYGY_NR12_WRn, HATO_RSTn, BUS_CPU_D02p.qp_new());
  /*#p11.JAFY*/ ch1.JAFY_NR12_ENV_DIR .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D03p.qp_new());
  /*#p11.JATY*/ ch1.JATY_NR12_VOL0    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D04p.qp_new());
  /*#p11.JAXO*/ ch1.JAXO_NR12_VOL1    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D05p.qp_new());
  /*#p11.JENA*/ ch1.JENA_NR12_VOL2    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D06p.qp_new());
  /*#p11.JOPU*/ ch1.JOPU_NR12_VOL3    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D07p.qp_new());

  /*#p11.HAMY*/ wire HAMY_NR12_RDn = or2(HAXE_ADDR_FF12n(), AGUZ_CPU_RDn());
  /*#p11.HOCU*/ wire HOCU_NR12_RDn = or2(GAGO_ADDR_FF12n(), AGUZ_CPU_RDn());

  // data polarity backwards again?
  /*#p11.JYNE*/ triwire JYNE = tri6_nn(HAMY_NR12_RDn, ch1.JUSA_NR12_DELAY0n.qp_new());
  /*#p11.JACA*/ triwire JACA = tri6_nn(HAMY_NR12_RDn, ch1.JUZY_NR12_DELAY1n.qp_new());
  /*#p11.JOKU*/ triwire JOKU = tri6_nn(HAMY_NR12_RDn, ch1.JOMA_NR12_DELAY2n.qp_new());
  /*#p11.HONO*/ triwire HONO = tri6_nn(HOCU_NR12_RDn, ch1.JAFY_NR12_ENV_DIR.qp_new());
  /*#p11.HOWU*/ triwire HOWU = tri6_nn(HOCU_NR12_RDn, ch1.JATY_NR12_VOL0.qp_new());
  /*#p11.HEWA*/ triwire HEWA = tri6_nn(HOCU_NR12_RDn, ch1.JAXO_NR12_VOL1.qp_new());
  /*#p11.HEVE*/ triwire HEVE = tri6_nn(HOCU_NR12_RDn, ch1.JENA_NR12_VOL2.qp_new());
  /*#p11.JYSE*/ triwire JYSE = tri6_nn(HOCU_NR12_RDn, ch1.JOPU_NR12_VOL3.qp_new());

  /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(JYNE);
  /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(JACA);
  /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(JOKU);
  /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(HONO);
  /*_BUS_CPU_D04p*/ BUS_CPU_D04p.tri_bus(HOWU);
  /*_BUS_CPU_D05p*/ BUS_CPU_D05p.tri_bus(HEWA);
  /*_BUS_CPU_D06p*/ BUS_CPU_D06p.tri_bus(HEVE);
  /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(JYSE);
}

//-----------------------------------------------------------------------------

void GBSound::tick_nr21() {
  /*_p15.DEME*/ wire DEME = nor3(ch2.CYRE_CH2_LEN_DONEp.qp_new(), BUFY_CLK_256n(), ch2.EMER_NR24_LENENp.qp_new()); // why was this or2?
  /*_p15.DYRO*/ wire DYRO_CH2_LEN_CLK = not1(DEME);

  /*_p10.DAZA*/ wire DAZA_ADDR_0110n = nand4(ACOL_A03n(), DENO_A02p(), DUPA_A01p(), DYTE_A00n());
  /*_p10.COVY*/ wire COVY_ADDR_FF16p = nor2(BANU_ADDR_FF1Xn(), DAZA_ADDR_0110n);
  /*_p14.AGYN*/ wire AGYN_FF16_WRn = nand2(BOGY_CPU_WRp(), COVY_ADDR_FF16p);
  /*_p15.BYMO*/ wire BYMO_FF16_WRp = not1(AGYN_FF16_WRn);
  /*_p15.AGET*/ wire AGET_FF16_WRp = not1(AGYN_FF16_WRn);
  /*_p14.ASYP*/ wire ASYP_FF16_WRp = not1(AGYN_FF16_WRn);
  /*_p14.BACU*/ wire BACU_FF16_WRp = and2(COVY_ADDR_FF16p, BOGY_CPU_WRp());
  /*_p14.BUDU*/ wire BUDU_FF16_WRn = not1(BACU_FF16_WRp);

  /*_p15.ERYC*/ ch2.ERYC_NR21_L0.dff20(DYRO_CH2_LEN_CLK,          BYMO_FF16_WRp, BUS_CPU_D00p.out_old());
  /*_p15.CERA*/ ch2.CERA_NR21_L1.dff20(ch2.ERYC_NR21_L0.qp_new(), BYMO_FF16_WRp, BUS_CPU_D01p.out_old());
  /*_p15.CONU*/ ch2.CONU_NR21_L2.dff20(ch2.CERA_NR21_L1.qp_new(), BYMO_FF16_WRp, BUS_CPU_D02p.out_old());
  /*_p15.CAME*/ ch2.CAME_NR21_L3.dff20(ch2.CONU_NR21_L2.qp_new(), BYMO_FF16_WRp, BUS_CPU_D03p.out_old());

  /*#p15.BUKO*/ wire BUKO_CH2_LEN_CLK = not1(ch2.CAME_NR21_L3.qn_new());
  /*_p15.BUVA*/ ch2.BUVA_NR21_L4.dff20(BUKO_CH2_LEN_CLK,      AGET_FF16_WRp, BUS_CPU_D04p.out_old());
  /*_p15.AKYD*/ ch2.AKYD_NR21_L5.dff20(ch2.BUVA_NR21_L4.qp_new(), AGET_FF16_WRp, BUS_CPU_D05p.out_old());

  /*_p14.BERA*/ ch2.BERA_NR21_D6.dff9(BUDU_FF16_WRn, AFAT_APU_RSTn(), BUS_CPU_D06p.out_old());
  /*_p14.BAMY*/ ch2.BAMY_NR21_D7.dff9(BUDU_FF16_WRn, AFAT_APU_RSTn(), BUS_CPU_D07p.out_old());

  // CH2 length expire flag?
  /*_p14.BENY*/ wire BENY_RSTn = nor3(ASYP_FF16_WRp, KEBA_APU_RSTp(), ch2.ELOX_CH2_TRIGp.qp_new());
  /*_p15.CYRE*/ ch2.CYRE_CH2_LEN_DONEp.dff17(ch2.AKYD_NR21_L5.qn_new(), BENY_RSTn, ch2.CYRE_CH2_LEN_DONEp.qn_old());

  /*_p14.BYGO*/ wire BYGO_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*_p14.CORO*/ wire CORO_NR21_RDn = nand2(COVY_ADDR_FF16p, BYGO_CPU_RDp);

  /*_p14.CECY*/ triwire CECY_D6 = tri6_nn(CORO_NR21_RDn, ch2.BERA_NR21_D6.qp_new()); // polarity?
  /*_p14.CEKA*/ triwire CEKA_D7 = tri6_nn(CORO_NR21_RDn, ch2.BAMY_NR21_D7.qp_new());
  
  // No read circuit for length?

  BUS_CPU_D06p.tri_bus(CECY_D6);
  BUS_CPU_D07p.tri_bus(CEKA_D7);
}

//-----------------------------------------------------------------------------

void GBSound::tick_nr50() {

  /*_p09.BOSU*/ wire BOSU_NR50_WRn = nand2(CAFY_ADDR_FF24p(), BOGY_CPU_WRp());
  /*_p09.BAXY*/ wire BAXY_NR50_WRp = not1(BOSU_NR50_WRn);
  /*_p09.BOWE*/ wire BOWE_NR50_WRp = not1(BOSU_NR50_WRn);
  /*_p09.BUBU*/ wire BUBU_NR50_WRn = not1(BAXY_NR50_WRp);
  /*_p09.ATAF*/ wire ATAF_NR50_WRn = not1(BOWE_NR50_WRp);

  /*_p09.APEG*/ spu.APEG_NR50_VOL_L0   .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D00p.out_old());
  /*_p09.BYGA*/ spu.BYGA_NR50_VOL_L1   .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D01p.out_old());
  /*_p09.AGER*/ spu.AGER_NR50_VOL_L2   .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D02p.out_old());
  /*_p09.APOS*/ spu.APOS_NR50_VIN_TO_L .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D03p.out_old());
  /*_p09.BYRE*/ spu.BYRE_NR50_VOL_R0   .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D04p.out_old());
  /*_p09.BUMO*/ spu.BUMO_NR50_VOL_R1   .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D05p.out_old());
  /*_p09.COZU*/ spu.COZU_NR50_VOL_R2   .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D06p.out_old());
  /*_p09.BEDU*/ spu.BEDU_NR50_VIN_TO_R .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn(), BUS_CPU_D07p.out_old());

  /*_p09.BEFU*/ wire BEFU_NR50_RDp = nor2(AGUZ_CPU_RDn(), BYMA_ADDR_FF24n());
  /*_p09.ADAK*/ wire ADAK_NR50_RDn = not1(BEFU_NR50_RDp);

  /*_p09.AKOD*/ triwire AKOD_D00 = tri6_nn(ADAK_NR50_RDn, spu.APEG_NR50_VOL_L0.qp_new()); // polarity?
  /*_p09.AWED*/ triwire AWED_D01 = tri6_nn(ADAK_NR50_RDn, spu.BYGA_NR50_VOL_L1.qp_new());
  /*_p09.AVUD*/ triwire AVUD_D02 = tri6_nn(ADAK_NR50_RDn, spu.AGER_NR50_VOL_L2.qp_new());
  /*_p09.AXEM*/ triwire AXEM_D03 = tri6_nn(ADAK_NR50_RDn, spu.APOS_NR50_VIN_TO_L.qp_new());
  /*_p09.AMAD*/ triwire AMAD_D04 = tri6_nn(ADAK_NR50_RDn, spu.BYRE_NR50_VOL_R0.qp_new());
  /*_p09.ARUX*/ triwire ARUX_D05 = tri6_nn(ADAK_NR50_RDn, spu.BUMO_NR50_VOL_R1.qp_new());
  /*_p09.BOCY*/ triwire BOCY_D06 = tri6_nn(ADAK_NR50_RDn, spu.COZU_NR50_VOL_R2.qp_new());
  /*_p09.ATUM*/ triwire ATUR_D07 = tri6_nn(ADAK_NR50_RDn, spu.BEDU_NR50_VIN_TO_R.qp_new());

  /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(AKOD_D00);
  /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(AWED_D01);
  /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(AVUD_D02);
  /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(AXEM_D03);
  /*_BUS_CPU_D04p*/ BUS_CPU_D04p.tri_bus(AMAD_D04);
  /*_BUS_CPU_D05p*/ BUS_CPU_D05p.tri_bus(ARUX_D05);
  /*_BUS_CPU_D06p*/ BUS_CPU_D06p.tri_bus(BOCY_D06);
  /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(ATUR_D07);
}

//-----------------------------------------------------------------------------

void GBSound::tick_nr51() {
  /*_p09.BUPO*/ wire BUPO_NR51_WRn = nand2(CORA_ADDR_FF25p(), BOGY_CPU_WRp());
  /*_p09.BONO*/ wire BONO_NR51_WRp = not1(BUPO_NR51_WRn);
  /*_p09.BYFA*/ wire BYFA_NR51_WRp = not1(BUPO_NR51_WRn);

  /*_p09.ANEV*/ spu.ANEV_NR51_D0.dff9(BONO_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D00p.out_old());
  /*_p09.BOGU*/ spu.BOGU_NR51_D1.dff9(BONO_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D01p.out_old());
  /*_p09.BAFO*/ spu.BAFO_NR51_D2.dff9(BONO_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D02p.out_old());
  /*_p09.ATUF*/ spu.ATUF_NR51_D3.dff9(BONO_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D03p.out_old());
  /*_p09.BUME*/ spu.BUME_NR51_D4.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D04p.out_old());
  /*_p09.BOFA*/ spu.BOFA_NR51_D5.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D05p.out_old());
  /*_p09.BEFO*/ spu.BEFO_NR51_D6.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D06p.out_old());
  /*_p09.BEPU*/ spu.BEPU_NR51_D7.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn(), BUS_CPU_D07p.out_old());

  /*_p09.HEFA*/ wire HEFA_NR51_RDp = nor2(GEPA_ADDR_FF25n(), AGUZ_CPU_RDn()); // outline wrong color in die
  /*_p09.GUMU*/ wire GUMU_NR51_RDn = not1(HEFA_NR51_RDp);

  /*_p09.BUZU*/ triwire BUZU_D0 = tri6_nn(GUMU_NR51_RDn, spu.ANEV_NR51_D0.qn_new());
  /*_p09.CAPU*/ triwire CAPU_D1 = tri6_nn(GUMU_NR51_RDn, spu.BOGU_NR51_D1.qn_new());
  /*_p09.CAGA*/ triwire CAGA_D2 = tri6_nn(GUMU_NR51_RDn, spu.BAFO_NR51_D2.qn_new());
  /*_p09.BOCA*/ triwire BOCA_D3 = tri6_nn(GUMU_NR51_RDn, spu.ATUF_NR51_D3.qn_new());
  /*_p09.CAVU*/ triwire CAVU_D4 = tri6_nn(GUMU_NR51_RDn, spu.BUME_NR51_D4.qn_new());
  /*_p09.CUDU*/ triwire CUDU_D5 = tri6_nn(GUMU_NR51_RDn, spu.BOFA_NR51_D5.qn_new());
  /*_p09.CADA*/ triwire CADA_D6 = tri6_nn(GUMU_NR51_RDn, spu.BEFO_NR51_D6.qn_new());
  /*_p09.CERE*/ triwire CERE_D7 = tri6_nn(GUMU_NR51_RDn, spu.BEPU_NR51_D7.qn_new());

  /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(BUZU_D0);
  /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(CAPU_D1);
  /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(CAGA_D2);
  /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(BOCA_D3);
  /*_BUS_CPU_D04p*/ BUS_CPU_D04p.tri_bus(CAVU_D4);
  /*_BUS_CPU_D05p*/ BUS_CPU_D05p.tri_bus(CUDU_D5);
  /*_BUS_CPU_D06p*/ BUS_CPU_D06p.tri_bus(CADA_D6);
  /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(CERE_D7);
}

//-----------------------------------------------------------------------------

void GBSound::tick_nr52() {
  // something not right here

  /*#p10.DOXY*/ wire DOXY_ADDR_FF26p = and2(CONA_ADDR_FF2Xp(), DOSO_A00p());

  /*#p09.ETUC*/ wire ETUC_NR52_WRp = and2(BOGY_CPU_WRp(), DOXY_ADDR_FF26p);
  /*#p09.FOKU*/ wire FOKU_NR52_WRn = not1(ETUC_NR52_WRp);
  /*_p09.BOPY*/ wire BOPY_NR52_WRn = nand2(DOXY_ADDR_FF26p, BOGY_CPU_WRp());
  /*_p09.HAWU*/ wire HAWU_NR52_WRn = nand2(DOXY_ADDR_FF26p, BOGY_CPU_WRp());

  /*_p09.EFOP*/ wire EFOP_NR52_DBG_APU_IN = and2(BUS_CPU_D04p.out_old(), UNOR_MODE_DBG2);
  /*_p09.FERO*/ spu.FERO_NR52_DBG_APUn    .dff9 (FOKU_NR52_WRn, KEPY_APU_RSTn(), EFOP_NR52_DBG_APU_IN);
  /*_p09.BOWY*/ spu.BOWY_NR52_DBG_SWEEP   .dff17(BOPY_NR52_WRn, KEPY_APU_RSTn(), BUS_CPU_D05p.out_old());
  /*_p09.HADA*/ spu.HADA_NR52_ALL_SOUND_ON.dff17(HAWU_NR52_WRn, GUFO_SYS_RSTn(), BUS_CPU_D07p.out_old()); // Since this bit controls APU_RESET*, it is reset by SYS_RESET.

  /*_p09.CETO*/ wire CETO_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*_p09.KAZO*/ wire KAZO_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*_p09.CURU*/ wire CURU_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*_p09.KYDU*/ wire KYDU_CPU_RDp = not1(AGUZ_CPU_RDn());

  /*_p09.DOLE*/ wire DOLE_NR52_RDn = nand2(DOXY_ADDR_FF26p, CETO_CPU_RDp);
  /*_p09.DURU*/ wire DURU_NR52_RDn = nand2(DOXY_ADDR_FF26p, CURU_CPU_RDp);
  /*_p09.FEWA*/ wire FEWA_NR52_RDn = nand2(DOXY_ADDR_FF26p, GAXO_CPU_RDp());
  /*_p09.KAMU*/ wire KAMU_NR52_RDn = nand2(DOXY_ADDR_FF26p, KAZO_CPU_RDp);
  /*_p09.JURE*/ wire JURE_NR52_RDn = nand2(DOXY_ADDR_FF26p, KYDU_CPU_RDp);

  /*#p18.COKA*/ wire COKA_CH3_ACTIVEp = not1(ch3.DAVO_CH3_ACTIVEp.qn_new());

  /*_p13.CARA*/ wire CARA_CH1_ACTIVEn = not1(ch1.CYTO_CH1_ACTIVEp.qp_new());
  /*_p15.DEFU*/ wire DEFU_CH2_ACTIVEn = not1(ch2.DANE_CH2_ACTIVEp.qp_new());
  /*_p18.ERED*/ wire ERED_CH3_ACTIVEn = not1(COKA_CH3_ACTIVEp);
  /*_p20.JUWA*/ wire JUWA_CH4_ACTIVEn = not1(ch4.GENA_CH4_ACTIVEp.qp_new());

  /*_p09.COTO*/ triwire COTO_D0 = tri6_nn(DOLE_NR52_RDn, CARA_CH1_ACTIVEn);
  /*_p09.EFUS*/ triwire EFUS_D1 = tri6_nn(DURU_NR52_RDn, DEFU_CH2_ACTIVEn);
  /*_p09.FATE*/ triwire FATE_D2 = tri6_nn(FEWA_NR52_RDn, ERED_CH3_ACTIVEn);
  /*_p09.KOGE*/ triwire KOGE_D3 = tri6_nn(KAMU_NR52_RDn, JUWA_CH4_ACTIVEn);
  /*_p09.HOPE*/ triwire HOPE_D7 = tri6_nn(JURE_NR52_RDn, spu.HADA_NR52_ALL_SOUND_ON.qn_new());

  /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(COTO_D0);
  /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(EFUS_D1);
  /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(FATE_D2);
  /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(KOGE_D3);
  /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(HOPE_D7);
}

















































//-----------------------------------------------------------------------------

void GBSound::tick() {

  /*_p16.ANUJ*/ wire ANUJ_CPU_WR_WEIRD = and2(SIG_IN_CPU_DBUS_FREE, BOGY_CPU_WRp());

  // Debug mode read
  /*_p09.EDEK*/ wire EDEK_NR52_DBG_APUn = not1(spu.FERO_NR52_DBG_APUn.qn_new());
  /*_p14.FOGE*/ wire FOGE_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*_p16.DOVO*/ wire DOVO_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*#p11.CEGE*/ wire CEGE_CPU_RDp = not1(AGUZ_CPU_RDn());
  /*_p14.FAPE*/ wire FAPE_CPU_RDp_DBG =  and2(FOGE_CPU_RDp, EDEK_NR52_DBG_APUn);
  /*_p16.EGAD*/ wire EGAD_CPU_RDn_DBG = nand2(DOVO_CPU_RDp, EDEK_NR52_DBG_APUn);
  /*#p11.DAXA*/ wire DAXA_CPU_RDn_DBG = nand2(CEGE_CPU_RDp, EDEK_NR52_DBG_APUn);

  //----------
  // SPU clock dividers


  /*_p01.CERY*/ spu.CERY_2M.dff17(CYBO_AxCxExGx(),      BELA_APU_RSTn(), spu.CERY_2M.qn_old());
  /*_p01.ATYK*/ spu.ATYK_2M.dff17(ARYF_AxCxExGx(),      BOPO_APU_RSTn(), spu.ATYK_2M.qn_old());
  /*_p09.AJER*/ spu.AJER_2M.dff17(APUV_AxCxExGx(),      ATYV_APU_RSTn(), spu.AJER_2M.qn_old());
  /*_p01.AVOK*/ spu.AVOK_1M.dff17(spu.ATYK_2M.qn_new(), BOPO_APU_RSTn(), spu.AVOK_1M.qn_old());

  /*#p01.BATA*/ wire BATA_CLK_2M = not1(spu.AJER_2M.qp_new());
  /*#p09.CALO*/ ch1.CALO_CLK_1M.dff17(BATA_CLK_2M, AGUR_APU_RSTn(), ch1.CALO_CLK_1M.qn_new());
  /*#p09.DYFA*/ wire DYFA_CLK_1M = not1(ch1.CALO_CLK_1M.qn_new()); // ch1

  /*_p01.BAVU*/ wire BAVU_1M = not1(spu.AVOK_1M.qp_new());

  /*_p01.JESO*/ spu.JESO_CLK_512K.dff17(BAVU_1M, KAME_APU_RSTn(), spu.JESO_CLK_512K.qn_old());

  /*_p15.ATEP*/ spu.ATEP_2MHZ.dff17(AZEG(), BUWE_APU_RSTn(), spu.ATEP_2MHZ.qn_old());
  /*#p15.BUFO*/   wire BUFO = not1(spu.ATEP_2MHZ.qp_new());
  /*_p15.BYHO*/   wire BYHO_APU_RSTp = not1(BUWE_APU_RSTn());
  /*#p15.CEMO*/ spu.CEMO_1MHZ.dff17(BUFO, BYHO_APU_RSTp, spu.CEMO_1MHZ.qn_old());

  //----------
  // Low-speed clocks are picked up from DIV

  /*_p01.COKE*/ wire COKE_2M = not1(spu.AJER_2M.qn_new());
  /*_p01.BARA*/ spu.BARA_CLK_512.dff17(COKE_2M, ATUS_APU_RSTn(), UMER_DIV10n_old);
  /*_p01.CARU*/ spu.CARU_CLK_256.dff17(BURE_CLK_512(), ATUS_APU_RSTn(), spu.CARU_CLK_256.qn_old());
  /*_p01.BYLU*/ spu.BYLU_CLK_128.dff17(spu.CARU_CLK_256.qn_new(), ATUS_APU_RSTn(), spu.BYLU_CLK_128.qn_old());

  //---------
  tick_nr50();

  //----------
  // FF26 NR52

  /*#p18.COKA*/ wire COKA_CH3_ACTIVEp = not1(ch3.DAVO_CH3_ACTIVEp.qn_new());














































  //========================================
  //========================================
  //========================================
  //========================================
  //             CHANNEL 2
  //========================================
  //========================================
  //========================================
  //========================================

  //----------
  // FF17 NR22

  {

    // FIXME wtf with the different clock polarities? i guess it doesn't matter much.
    /*#p14.ENUF*/ wire ENUF = and2(DUTU_ADDR_FF17p(), BOGY_CPU_WRp());
    /*#p14.GERE*/ wire GERE = and2(BOGY_CPU_WRp(), DUTU_ADDR_FF17p());
    /*#p14.JEDE*/ wire JEDE = not1(GERE);

    /*_p14.HYFU*/ ch2.HYFU_NR22_P0n.dff9(JEDE, JYBU_APU_RSTn(), BUS_CPU_D00p.out_old());
    /*_p14.HORE*/ ch2.HORE_NR22_P1n.dff9(JEDE, JYBU_APU_RSTn(), BUS_CPU_D01p.out_old());
    /*_p14.HAVA*/ ch2.HAVA_NR22_P2n.dff9(JEDE, JYBU_APU_RSTn(), BUS_CPU_D02p.out_old());
    /*_p14.FORE*/ ch2.FORE_NR22_ADD.dff9(ENUF, JYBU_APU_RSTn(), BUS_CPU_D03p.out_old());
    /*_p14.GATA*/ ch2.GATA_NR22_V0.dff9(ENUF, JYBU_APU_RSTn(), BUS_CPU_D04p.out_old());
    /*_p14.GUFE*/ ch2.GUFE_NR22_V1.dff9(ENUF, JYBU_APU_RSTn(), BUS_CPU_D05p.out_old());
    /*_p14.GURA*/ ch2.GURA_NR22_V2.dff9(ENUF, JYBU_APU_RSTn(), BUS_CPU_D06p.out_old());
    /*_p14.GAGE*/ ch2.GAGE_NR22_V3.dff9(ENUF, JYBU_APU_RSTn(), BUS_CPU_D07p.out_old());

    /*_p14.GURE*/ wire GURE = not1(DUTU_ADDR_FF17p());
    /*_p14.FYRY*/ wire FYRY = not1(DUTU_ADDR_FF17p());
    /*_p14.GEXA*/ wire GEXA = or2(GURE, AGUZ_CPU_RDn());
    /*_p14.GURU*/ wire GURU = or2(FYRY, AGUZ_CPU_RDn());

    /*_p14.HUVU*/ triwire HUVU_D0 = tri6_nn(GEXA, ch2.HYFU_NR22_P0n.qp_new());
    /*_p14.HYRE*/ triwire HYRE_D1 = tri6_nn(GEXA, ch2.HORE_NR22_P1n.qp_new());
    /*_p14.HAVU*/ triwire HAVU_D2 = tri6_nn(GEXA, ch2.HYFU_NR22_P0n.qp_new());
    /*_p14.GENE*/ triwire GENE_D3 = tri6_nn(GURU, ch2.FORE_NR22_ADD.qp_new());
    /*_p14.HUPE*/ triwire HUPE_D4 = tri6_nn(GURU, ch2.GATA_NR22_V0.qp_new());
    /*_p14.HERE*/ triwire HERE_D5 = tri6_nn(GURU, ch2.GUFE_NR22_V1.qp_new());
    /*_p14.HORO*/ triwire HORO_D6 = tri6_nn(GURU, ch2.GURA_NR22_V2.qp_new());
    /*_p14.HYRY*/ triwire HYRY_D7 = tri6_nn(GURU, ch2.GAGE_NR22_V3.qp_new());

    BUS_CPU_D00p.tri_bus(HUVU_D0);
    BUS_CPU_D01p.tri_bus(HYRE_D1);
    BUS_CPU_D02p.tri_bus(HAVU_D2);
    BUS_CPU_D03p.tri_bus(GENE_D3);
    BUS_CPU_D04p.tri_bus(HUPE_D4);
    BUS_CPU_D05p.tri_bus(HERE_D5);
    BUS_CPU_D06p.tri_bus(HORO_D6);
    BUS_CPU_D07p.tri_bus(HYRY_D7);
  }

  //----------
  // FF18 NR23

  {
    /*_p14.DOSA*/ wire DOSA_NR23_WR1 = and2(DARA_ADDR_FF18p(), BOGY_CPU_WRp());
    /*_p14.EXUC*/ wire EXUC_NR23_WR2 = and2(DARA_ADDR_FF18p(), BOGY_CPU_WRp());
    /*_p14.ESUR*/ wire ESUR_NR23_WRn1 = not1(DOSA_NR23_WR1);
    /*_p14.FYXO*/ wire FYXO_NR23_WRn2 = not1(EXUC_NR23_WR2);
    /*_p14.FOFE*/ ch2.FOFE_NR23_FREQ_00.dff9(ESUR_NR23_WRn1, HUDE_APU_RSTn(), BUS_CPU_D00p.out_old());
    /*_p14.FOVA*/ ch2.FOVA_NR23_FREQ_01.dff9(ESUR_NR23_WRn1, HUDE_APU_RSTn(), BUS_CPU_D01p.out_old());
    /*_p14.FEDY*/ ch2.FEDY_NR23_FREQ_02.dff9(ESUR_NR23_WRn1, HUDE_APU_RSTn(), BUS_CPU_D02p.out_old());
    /*_p14.FOME*/ ch2.FOME_NR23_FREQ_03.dff9(ESUR_NR23_WRn1, HUDE_APU_RSTn(), BUS_CPU_D03p.out_old());
    /*_p14.FORA*/ ch2.FORA_NR23_FREQ_04.dff9(ESUR_NR23_WRn1, HUDE_APU_RSTn(), BUS_CPU_D04p.out_old());
    /*_p14.GODA*/ ch2.GODA_NR23_FREQ_05.dff9(FYXO_NR23_WRn2, HUDE_APU_RSTn(), BUS_CPU_D05p.out_old());
    /*_p14.GUMY*/ ch2.GUMY_NR23_FREQ_06.dff9(FYXO_NR23_WRn2, HUDE_APU_RSTn(), BUS_CPU_D06p.out_old());
    /*_p14.GUPU*/ ch2.GUPU_NR23_FREQ_07.dff9(FYXO_NR23_WRn2, HUDE_APU_RSTn(), BUS_CPU_D07p.out_old());

    // this can't be right...

    /*_p14.FERY*/ wire FERY = not1(DUCE_A03p());
    /*_p14.GUZA*/ wire GUZA = nor2(FERY, FAPE_CPU_RDp_DBG);
    /*_p14.FUTY*/ wire FUTY = not1(GUZA);
    /*#p14.FAVA*/ triwire FAVA_D0 = tri6_nn(FUTY, ch2.DONE_CH2_FREQ_00.qn_new());
    /*#p14.FAJY*/ triwire FAJY_D1 = tri6_nn(FUTY, ch2.DYNU_CH2_FREQ_01.qn_new());
    /*#p14.FEGU*/ triwire FEGU_D2 = tri6_nn(FUTY, ch2.EZOF_CH2_FREQ_02.qn_new());
    /*#p14.FOSE*/ triwire FOSE_D3 = tri6_nn(FUTY, ch2.CYVO_CH2_FREQ_03.qn_new());
    /*#p14.GERO*/ triwire GERO_D4 = tri6_nn(FUTY, ch2.FUXO_CH2_FREQ_04.qn_new()); // d4 and d5 are switched on the schematic
    /*#p14.GAKY*/ triwire GAKY_D5 = tri6_nn(FUTY, ch2.GANO_CH2_FREQ_05.qn_new());
    /*#p14.GADU*/ triwire GADU_D6 = tri6_nn(FUTY, ch2.GOCA_CH2_FREQ_06.qn_new());
    /*#p14.GAZO*/ triwire GAZO_D7 = tri6_nn(FUTY, ch2.GANE_CH2_FREQ_07.qn_new());

    BUS_CPU_D00p.tri_bus(FAVA_D0);
    BUS_CPU_D01p.tri_bus(FAJY_D1);
    BUS_CPU_D02p.tri_bus(FEGU_D2);
    BUS_CPU_D03p.tri_bus(FOSE_D3);
    BUS_CPU_D04p.tri_bus(GERO_D4);
    BUS_CPU_D05p.tri_bus(GAKY_D5);
    BUS_CPU_D06p.tri_bus(GADU_D6);
    BUS_CPU_D07p.tri_bus(GAZO_D7);
  }

  //----------
  // FF19 NR24

  // Ch2 trigger / sequencer
  /*#p15.DOPE*/ ch2.DOPE_CH2_TRIG.dff17(DOVA_ABCDxxxx(), CYWU_APU_RSTn(), ch2.ETAP_NR24_TRIG.qn_old());
  /*_p15.DOXA*/ wire DOXA_RSTn = or2(KEBA_APU_RSTp(), ch2.DORY_CH2_TRIGp.qp_new()); // FIXME this doesn't look right
  /*_p15.CELO*/ wire CELO = not1(DOXA_RSTn);
  /*_p15.DALA*/ wire DALA = or2(CELO, ch2.DOPE_CH2_TRIG.qp_new());

  /*_p15.ELOX*/ ch2.ELOX_CH2_TRIGp.dff17(spu.CEMO_1MHZ.qp_new(), DOXA_RSTn,       DALA);
  /*_p15.DORY*/ ch2.DORY_CH2_TRIGp.dff17(spu.CEMO_1MHZ.qp_new(), CEXE_APU_RSTn(), ch2.ELOX_CH2_TRIGp.qp_old());
  /*_p15.CAZA*/ ch2.CAZA_CH2_TRIGp.dff17(spu.CEMO_1MHZ.qp_new(), CEXE_APU_RSTn(), ch2.DORY_CH2_TRIGp.qp_old());

  {
    /*_pXX.JENU*/ wire JENU_NR24_WRp = and2(DOZA_ADDR_FF19p(), BOGY_CPU_WRp());
    /*_pXX.KYSA*/ wire KYSA_NR24_WRn = not1(JENU_NR24_WRp);
    /*_p14.KYPU*/ wire KYPU_APU_RSTn = not1(KEBA_APU_RSTp());
    /*_pXX.JEFU*/ ch2.JEFU_NR24_FREQ_08.dff9(KYSA_NR24_WRn, KYPU_APU_RSTn, BUS_CPU_D00p.out_old());
    /*_pXX.JANY*/ ch2.JANY_NR24_FREQ_09.dff9(KYSA_NR24_WRn, KYPU_APU_RSTn, BUS_CPU_D01p.out_old());
    /*_pXX.JUPY*/ ch2.JUPY_NR24_FREQ_10.dff9(KYSA_NR24_WRn, KYPU_APU_RSTn, BUS_CPU_D02p.out_old());

    /*_pXX.EVYF*/ wire EVYF = nand2(ANUJ_CPU_WR_WEIRD, DOZA_ADDR_FF19p());
    /*_p14.FAZO*/ wire FAZO_APU_RSTn = not1(KEBA_APU_RSTp());
    /*_pXX.EMER*/ ch2.EMER_NR24_LENENp.dff9(EVYF, FAZO_APU_RSTn, BUS_CPU_D06p.out_old());

    /*#pXX.GADO*/ wire GADO = not1(AGUZ_CPU_RDn());
    /*#pXX.HUMA*/ wire HUMA = nand2(DOZA_ADDR_FF19p(), GADO); // why was this nor2? doublecheck
    /*#pXX.GOJY*/ triwire GOJY_D6 = tri6_nn(HUMA, ch2.EMER_NR24_LENENp.qp_new());
    BUS_CPU_D06p.tri_bus(GOJY_D6);

    /*#p??.DETA*/ wire DETA_NR24_WRn = nand2(BOGY_CPU_WRp(), DOZA_ADDR_FF19p());
    /*#p15.DERA*/ wire DERA_RSTn = nor2(KEBA_APU_RSTp(), ch2.DOPE_CH2_TRIG.qp_new());
    /*#p??.ETAP*/ ch2.ETAP_NR24_TRIG.dff9(DETA_NR24_WRn, DERA_RSTn, BUS_CPU_D07p.out_old());

    /*#p14.GOTE*/ wire GOTE = not1(DOZA_ADDR_FF19p());
    /*#p14.HYPO*/ wire HYPO = or2(GOTE, FAPE_CPU_RDp_DBG);
    /*#p14.HUNA*/ triwire HUNA_D0 = tri6_nn(HYPO, ch2.HEVY_CH2_FREQ_08.qn_new());
    /*#p14.JARO*/ triwire JARO_D1 = tri6_nn(HYPO, ch2.HEPU_CH2_FREQ_09.qn_new());
    /*#p14.JEKE*/ triwire JEKE_D2 = tri6_nn(HYPO, ch2.HERO_CH2_FREQ_10.qn_new());

    BUS_CPU_D00p.tri_bus(HUNA_D0);
    BUS_CPU_D01p.tri_bus(JARO_D1);
    BUS_CPU_D02p.tri_bus(JEKE_D2);
  }

  //----------

  /*#p15.FUTE*/ wire FUTE_CH2_AMP_ENn = nor5(ch2.FORE_NR22_ADD.qn_new(), ch2.GATA_NR22_V0.qn_new(), ch2.GUFE_NR22_V1.qn_new(), ch2.GURA_NR22_V2.qn_new(), ch2.GAGE_NR22_V3.qn_new());

  // this is freq overflow or something
  /*#p15.FUJY*/ wire FUJY = and2(ch2.GYKO_CLK.qp_new(), spu.CEMO_1MHZ.qp_new());
  /*#p15.GALU*/ wire GALU_CH2_FREQ_OVERFLOWp = not1(ch2.HERO_CH2_FREQ_10.qp_new());
  /*#p15.GYRE*/ wire GYRE = nor3(KEBA_APU_RSTp(), ch2.ELOX_CH2_TRIGp.qp_new(), FUJY);
  /*#p15.GYKO*/ ch2.GYKO_CLK.dff17(GALU_CH2_FREQ_OVERFLOWp, GYRE, ch2.GYKO_CLK.qn_old());
  /*#p15.ETUK*/ wire ETUK_CLK = not1(ch2.GYKO_CLK.qp_new());
  /*#p15.DAVU*/ wire DAVU_CLK = not1(ETUK_CLK);


  // Channel 2 frequency counter
  {
    /*#p15.ARES*/ wire ARES = nor2(FUTE_CH2_AMP_ENn, KEBA_APU_RSTp()); // looks like nor on the die?
    /*#p15.BODO*/ wire BODO = not1(ch2.CAZA_CH2_TRIGp.qp_new());
    /*_p15.BUTA*/ ch2.BUTA_FREQ_GATEp.nand_latch(ARES, BODO);
    /*#p15.CAMA*/ wire CAMA_FREQ_TICKp = nor2(spu.CEMO_1MHZ.qp_new(), ch2.BUTA_FREQ_GATEp.qp_new()); // no idea of buta polarity
    /*_p15.DOCA*/ wire DOCA_FREQ_TICKn = not1(CAMA_FREQ_TICKp);
    /*#p15.DUJU*/ wire DUJU_LOADn = nor2(DAVU_CLK, ch2.ELOX_CH2_TRIGp.qp_new());

    /*#p15.COGU*/ wire COGU_LOADp = not1(DUJU_LOADn);
    /*_p14.DONE*/ ch2.DONE_CH2_FREQ_00.dff20(DOCA_FREQ_TICKn,                COGU_LOADp, ch2.FOFE_NR23_FREQ_00.qp_new());
    /*_p14.DYNU*/ ch2.DYNU_CH2_FREQ_01.dff20(ch2.DONE_CH2_FREQ_00.qp_new(),  COGU_LOADp, ch2.FOVA_NR23_FREQ_01.qp_new());
    /*_p14.EZOF*/ ch2.EZOF_CH2_FREQ_02.dff20(ch2.DYNU_CH2_FREQ_01.qp_new(),  COGU_LOADp, ch2.FEDY_NR23_FREQ_02.qp_new());
    /*_p14.CYVO*/ ch2.CYVO_CH2_FREQ_03.dff20(ch2.EZOF_CH2_FREQ_02.qp_new(),  COGU_LOADp, ch2.FOME_NR23_FREQ_03.qp_new());

    /*#p14.EDEP*/ wire EDEP_CH2_FREQ_03 = not1(ch2.CYVO_CH2_FREQ_03.qn_new());
    /*#p15.EROG*/ wire EROG_LOADp = not1(DUJU_LOADn);
    /*_p14.FUXO*/ ch2.FUXO_CH2_FREQ_04.dff20(EDEP_CH2_FREQ_03,               EROG_LOADp, ch2.FORA_NR23_FREQ_04.qp_new());
    /*_p14.GANO*/ ch2.GANO_CH2_FREQ_05.dff20(ch2.FUXO_CH2_FREQ_04.qp_new(),  EROG_LOADp, ch2.GODA_NR23_FREQ_05.qp_new());
    /*_p14.GOCA*/ ch2.GOCA_CH2_FREQ_06.dff20(ch2.GANO_CH2_FREQ_05.qp_new(),  EROG_LOADp, ch2.GUMY_NR23_FREQ_06.qp_new());
    /*_p14.GANE*/ ch2.GANE_CH2_FREQ_07.dff20(ch2.GOCA_CH2_FREQ_06.qp_new(),  EROG_LOADp, ch2.GUPU_NR23_FREQ_07.qp_new());

    /*#p14.GALA*/ wire GALA_CH2_FREQ_07 = not1(ch2.GANE_CH2_FREQ_07.qn_new());
    /*#p15.GYPA*/ wire GYPA_LOADp = not1(DUJU_LOADn);
    /*_p14.HEVY*/ ch2.HEVY_CH2_FREQ_08.dff20(GALA_CH2_FREQ_07,               GYPA_LOADp, ch2.JEFU_NR24_FREQ_08.qp_new());
    /*_p14.HEPU*/ ch2.HEPU_CH2_FREQ_09.dff20(ch2.HEVY_CH2_FREQ_08.qp_new(),  GYPA_LOADp, ch2.JANY_NR24_FREQ_09.qp_new());
    /*_p14.HERO*/ ch2.HERO_CH2_FREQ_10.dff20(ch2.HEPU_CH2_FREQ_09.qp_new(),  GYPA_LOADp, ch2.JUPY_NR24_FREQ_10.qp_new());
  }

  /*#p15.JUPU*/ wire JUPU_ENV_OFFp = nor3(ch2.HYFU_NR22_P0n.qn_new(), ch2.HORE_NR22_P1n.qn_new(), ch2.HAVA_NR22_P2n.qn_new());
 
  {
    /*#p15.HOTA*/ wire HOTA_CLK_128 = not1(BYFE_CLK_128n());
    /*_p15.KATY*/ wire KATY_APU_RSTn = not1(KEBA_APU_RSTp());
    /*#p15.JYNA*/ ch2.JYNA_CLK_64n.dff17(HOTA_CLK_128, KATY_APU_RSTn, ch2.JYNA_CLK_64n.qn_old());

    /*#p15.KYLO*/ wire KYLO_CLK_64p = not1(ch2.JYNA_CLK_64n.qp_new());
    /*#p15.KENE*/ wire KENE_CLK_64n = not1(KYLO_CLK_64p);
    
    /*_p15.HYLY*/ wire HYLY_ENV_LOADn = nor2(ch2.ELOX_CH2_TRIGp.qp_new(), ch2.JOPA_CH2_ENV_TICK.qp_new());
    /*_p15.JAKE*/ wire JAKE_ENV_LOADp = not1(HYLY_ENV_LOADn);
    
    /*#p15.JORE*/ ch2.JORE_CH2_ENV_DELAYp.dff20(KENE_CLK_64n,                     JAKE_ENV_LOADp, ch2.HYFU_NR22_P0n.qp_new());
    /*#p15.JONA*/ ch2.JONA_CH2_ENV_DELAYp.dff20(ch2.JORE_CH2_ENV_DELAYp.qp_new(), JAKE_ENV_LOADp, ch2.HORE_NR22_P1n.qp_new());
    /*#p15.JEVY*/ ch2.JEVY_CH2_ENV_DELAYp.dff20(ch2.JONA_CH2_ENV_DELAYp.qp_new(), JAKE_ENV_LOADp, ch2.HAVA_NR22_P2n.qp_new());

    /*#p15.GADE*/ wire GADE = not1(ch2.JOPA_CH2_ENV_TICK.qp_new());
    /*#p15.HOLY*/ wire HOLY_CLKp = nor2(HORU_CLK_512p(), GADE);
    /*#p15.HAFE*/ wire HAFE_RSTn = nor4(HOLY_CLKp, JUPU_ENV_OFFp, ch2.ELOX_CH2_TRIGp.qp_new(), KEBA_APU_RSTp()); // schematic wrong this is nor4
    /*#p15.KYVO*/ wire KYVO_CH2_ENV_TICK = and3(ch2.JEVY_CH2_ENV_DELAYp.qp_new(), ch2.JONA_CH2_ENV_DELAYp.qp_new(), ch2.JORE_CH2_ENV_DELAYp.qp_new());
    /*#p15.JOPA*/ ch2.JOPA_CH2_ENV_TICK.dff17(HORU_CLK_512p(), HAFE_RSTn, KYVO_CH2_ENV_TICK);
  }

  {
    /*_p15.HYPA*/ wire HYPA = nor2(ch2.ELOX_CH2_TRIGp.qp_new(), KEBA_APU_RSTp());
    /*#p15.EMYR*/ wire EMYR = nor5 (ch2.FORE_NR22_ADD.qn_new(), ch2.FENO_CH2_VOL0.qp_new(), ch2.FETE_CH2_VOL1.qp_new(), ch2.FOMY_CH2_VOL2.qp_new(), ch2.FENA_CH2_VOL3.qp_new());
    /*#p15.ERAT*/ wire ERAT = nand5(ch2.FORE_NR22_ADD.qn_new(), ch2.FENO_CH2_VOL0.qp_new(), ch2.FETE_CH2_VOL1.qp_new(), ch2.FOMY_CH2_VOL2.qp_new(), ch2.FENA_CH2_VOL3.qp_new());
    /*#p15.FYRE*/ wire FYRE = not1(ERAT);
    /*#p15.GUFY*/ wire GUFY = or2(EMYR, FYRE);
    /*#p15.HEPO*/ ch2.HEPO.dff17(ch2.JOPA_CH2_ENV_TICK.qp_new(), HYPA, GUFY);
    /*_p15.HYLE*/ wire HYLE = or2(KEBA_APU_RSTp(), ch2.ELOX_CH2_TRIGp.qp_new());
    /*#p15.JEME*/ ch2.JEME.nor_latch(ch2.HEPO.qp_new(), HYLE);
  }
  
  /*_p15.DORA*/ wire DORA_CH2_STOPp = and2(ch2.CYRE_CH2_LEN_DONEp.qp_new(), ch2.EMER_NR24_LENENp.qn_new());
  /*_p15.ESYK*/ wire ESYK_CH2_STOPp = or3(KEBA_APU_RSTp(), DORA_CH2_STOPp, FUTE_CH2_AMP_ENn);
  /*_p15.DANE*/ ch2.DANE_CH2_ACTIVEp.nor_latch(ch2.ELOX_CH2_TRIGp.qp_new(), ESYK_CH2_STOPp);

  {
    // CH2 volume? envelope?
    /*_p15.HOFO*/ wire HOFO = or3(ch2.JOPA_CH2_ENV_TICK.qp_new(), JUPU_ENV_OFFp, ch2.JEME.qp_new());
    /*#p15.GAFA*/ wire GAFA = amux2(ch2.FORE_NR22_ADD.qn_new(), HOFO,                       HOFO,                       ch2.FORE_NR22_ADD.qp_new());
    /*#p15.FARU*/ wire FARU = amux2(ch2.FORE_NR22_ADD.qn_new(), ch2.FENO_CH2_VOL0.qp_new(), ch2.FENO_CH2_VOL0.qn_new(), ch2.FORE_NR22_ADD.qp_new());
    /*#p15.ETUP*/ wire ETUP = amux2(ch2.FORE_NR22_ADD.qn_new(), ch2.FETE_CH2_VOL1.qp_new(), ch2.FETE_CH2_VOL1.qn_new(), ch2.FORE_NR22_ADD.qp_new());
    /*#p15.FOPY*/ wire FOPY = amux2(ch2.FORE_NR22_ADD.qn_new(), ch2.FOMY_CH2_VOL2.qp_new(), ch2.FOMY_CH2_VOL2.qn_new(), ch2.FORE_NR22_ADD.qp_new());

    /*#p15.FENO*/ ch2.FENO_CH2_VOL0.dff20(GAFA, ch2.ELOX_CH2_TRIGp.qp_new(), ch2.FENO_CH2_VOL0.qn_old());
    /*#p15.FETE*/ ch2.FETE_CH2_VOL1.dff20(FARU, ch2.ELOX_CH2_TRIGp.qp_new(), ch2.GUFE_NR22_V1.qn_old());
    /*#p15.FOMY*/ ch2.FOMY_CH2_VOL2.dff20(ETUP, ch2.ELOX_CH2_TRIGp.qp_new(), ch2.GURA_NR22_V2.qn_old());
    /*#p15.FENA*/ ch2.FENA_CH2_VOL3.dff20(FOPY, ch2.ELOX_CH2_TRIGp.qp_new(), ch2.GAGE_NR22_V3.qn_old());
  }

  {
    // CH2 duty cycle generator

    /*#p15.CULE*/ wire CULE_CLK = not1(DAVU_CLK);
    /*#p15.CANO*/ ch2.CANO_00.dff17(CULE_CLK,             AFAT_APU_RSTn(), ch2.CANO_00.qn_old());
    /*#p15.CAGY*/ ch2.CAGY_01.dff13(ch2.CANO_00.qn_new(), AFAT_APU_RSTn(), ch2.CAGY_01.qn_old());
    /*#p15.DYVE*/ ch2.DYVE_02.dff13(ch2.CAGY_01.qn_new(), AFAT_APU_RSTn(), ch2.DYVE_02.qn_old());

    /*#p15.DOJU*/ wire DOJU = nor2(ch2.BERA_NR21_D6.qn_new(), ch2.BAMY_NR21_D7.qp_new());
    /*#p15.DYTA*/ wire DYTA = nor2(ch2.BERA_NR21_D6.qp_new(), ch2.BAMY_NR21_D7.qn_new());
    /*#p15.DOMO*/ wire DOMO = nor2(ch2.BERA_NR21_D6.qn_new(), ch2.BAMY_NR21_D7.qn_new());
    /*#p15.DOVE*/ wire DOVE = nor2(ch2.BERA_NR21_D6.qp_new(), ch2.BAMY_NR21_D7.qp_new());
    /*#p15.DUGE*/ wire DUGE = not1(ch2.CANO_00.qp_new());
    /*#p15.DYMU*/ wire DYMU = and2(ch2.DYVE_02.qp_new(), ch2.CAGY_01.qp_new());
    /*#p15.EGOG*/ wire EGOG = and2(DYMU, DUGE);
    /*#p15.DARE*/ wire DARE = not1(DYMU);
    /*#p15.EXES*/ wire EXES_WAVE = amux4(EGOG, DOMO, DYMU, DYTA, ch2.DYVE_02.qp_new(), DOJU, DARE, DOVE);
    /*#p15.DOME*/ ch2.DOME_CH2_OUTp.dff17(DAVU_CLK, AFAT_APU_RSTn(), EXES_WAVE);
  }

  {
    // CH2 output to DAC
    // dac 2 inputs ANAN ANYV ASOG AMOV
    /*#p15.CYSE*/ wire CYSE_CH2_OUTp = and2(ch2.DANE_CH2_ACTIVEp.qp_new(), ch2.DOME_CH2_OUTp.qp_new());
    /*#p15.BONU*/ wire BONU_CH2_OUTp = or2(CYSE_CH2_OUTp, EDEK_NR52_DBG_APUn);
    /*#p15.ANAN*/ wire ANAN_CH2_BIT0 = and2(ch2.FENO_CH2_VOL0.qp_new(), BONU_CH2_OUTp);
    /*#p15.ANYV*/ wire ANYV_CH2_BIT1 = and2(ch2.FETE_CH2_VOL1.qp_new(), BONU_CH2_OUTp);
    /*#p15.ASOG*/ wire ASOG_CH2_BIT2 = and2(ch2.FOMY_CH2_VOL2.qp_new(), BONU_CH2_OUTp);
    /*#p15.AMOV*/ wire AMOV_CH2_BIT3 = and2(ch2.FENA_CH2_VOL3.qp_new(), BONU_CH2_OUTp);
  }
















































  //========================================
  //========================================
  //========================================
  //========================================
  //             CHANNEL 3
  //========================================
  //========================================
  //========================================
  //========================================


  //----------
  // FF1A NR30 - Channel 3 Sound on/off (R/W)

  {
    /*_p10.EXAT*/ wire EXAT_ADDR_1010n = nand4(DUCE_A03p(), ABUB_A02n(), DUPA_A01p(), DYTE_A00n());
    /*_p10.EMOR*/ wire EMOR_ADDR_FF1Ap = nor2(BANU_ADDR_FF1Xn(), EXAT_ADDR_1010n);

    /*_p16.GOVE*/ wire GOVE_APU_RESETn = not1(KEBA_APU_RSTp());
    /*_p16.GEJO*/ wire GEJO_FF1A_WRp = and2(BOGY_CPU_WRp(), EMOR_ADDR_FF1Ap);
    /*_p16.GUCY*/ wire GUCY_FF1A_WRn = not1(GEJO_FF1A_WRp);
    /*_p16.GUXE*/ ch3.GUXE_CH3_AMP_ENn.dff9(GUCY_FF1A_WRn, GOVE_APU_RESETn, BUS_CPU_D00p.out_old());

    /*_p16.FASY*/ wire FASY_FF1A_RD = nand2(EMOR_ADDR_FF1Ap, GAXO_CPU_RDp());
    /*_p16.FEVO*/ wire FEVO_CH3_AMP_ENa  = not1(ch3.GUXE_CH3_AMP_ENn.qn_new());

  #if 0
    if (FASY_FF1A_RD) set_data(
      /*_p16.GEKO*/ FEVO_CH3_AMP_ENa
    );
  #endif
  }

  //----------
  // FF1B - NR31 - Channel 3 Sound Length

  {
    /*_p10.EMAX*/ wire EMAX_ADDR_1011n = nand4(DOSO_A00p(), DUPA_A01p(), ABUB_A02n(), DUCE_A03p());
    /*_p10.DUSA*/ wire DUSA_ADDR_FF1Bp = nor2(BANU_ADDR_FF1Xn(), EMAX_ADDR_1011n);
    /*_p16.DERY*/ wire DERY_FF1B_WRn = nand2(BOGY_CPU_WRp(), DUSA_ADDR_FF1Bp);

    // fexu/hoto polarity seems wrong
    /*#p18.GEPY*/ wire GEPY_LEN_CLKp = nor3(ch3.FEXU_LEN_DONEp.qp_new(), BUFY_CLK_256n(), ch3.HOTO_NR34_LENEN.qp_new());
    /*#p18.GENU*/ wire GENU_LEN_CLKn = not1(GEPY_LEN_CLKp);
    /*_p16.GAJY*/ wire GAJY_LOADp = not1(DERY_FF1B_WRn);
    /*_p18.GEVO*/ ch3.GEVO_CH3_LEN0p.dff20(GENU_LEN_CLKn,               GAJY_LOADp, BUS_CPU_D00p.out_old());
    /*_p18.FORY*/ ch3.FORY_CH3_LEN1p.dff20(ch3.GEVO_CH3_LEN0p.qp_new(), GAJY_LOADp, BUS_CPU_D01p.out_old());
    /*_p18.GATU*/ ch3.GATU_CH3_LEN2p.dff20(ch3.FORY_CH3_LEN1p.qp_new(), GAJY_LOADp, BUS_CPU_D02p.out_old());
    /*_p18.GAPO*/ ch3.GAPO_CH3_LEN3p.dff20(ch3.GATU_CH3_LEN2p.qp_new(), GAJY_LOADp, BUS_CPU_D03p.out_old());

    /*#p18.FALU*/ wire FALU_LEN_CLKp = not1(ch3.GAPO_CH3_LEN3p.qn_new());
    /*_p16.EMUT*/ wire EMUT_LOADp = not1(DERY_FF1B_WRn);
    /*_p18.GEMO*/ ch3.GEMO_CH3_LEN4p.dff20(FALU_LEN_CLKp,               EMUT_LOADp, BUS_CPU_D04p.out_old());
    /*_p18.FORO*/ ch3.FORO_CH3_LEN5p.dff20(ch3.GEMO_CH3_LEN4p.qp_new(), EMUT_LOADp, BUS_CPU_D05p.out_old());
    /*_p18.FAVE*/ ch3.FAVE_CH3_LEN6p.dff20(ch3.FORO_CH3_LEN5p.qp_new(), EMUT_LOADp, BUS_CPU_D06p.out_old());
    /*_p18.FYRU*/ ch3.FYRU_CH3_LEN7p.dff20(ch3.FAVE_CH3_LEN6p.qp_new(), EMUT_LOADp, BUS_CPU_D07p.out_old());

    /*#p16.GETO*/ wire GETO_FF1B_WRp = not1(DERY_FF1B_WRn);
    /*#p18.GUDA*/ wire GUDA_LEN_DONE_RSTn = nor3(GETO_FF1B_WRp, KEBA_APU_RSTp(), ch3.GARA.qp_new());
    /*_p18.FEXU*/ ch3.FEXU_LEN_DONEp.dff17(ch3.FYRU_CH3_LEN7p.qn_new(), GUDA_LEN_DONE_RSTn, ch3.FEXU_LEN_DONEp.qn_old());
  }

  //----------
  // FF1C NR32 - Channel 3 Select output level (R/W)

  {
    /*_p10.GANY*/ wire GANY_ADDR_1100an = nand4(DUCE_A03p(), DENO_A02p(), AFOB_A01n(), DYTE_A00n());
    /*_p10.GEFO*/ wire GEFO_ADDR_FF1C  = nor2(BANU_ADDR_FF1Xn(), GANY_ADDR_1100an);

    /*_p16.GURO*/ wire GURO_APU_RESETn = not1(KEBA_APU_RSTp());

    /*_p16.HAGA*/ wire HAGA_FF1C_WR = and2(BOGY_CPU_WRp(), GEFO_ADDR_FF1C);

    /*_p16.GUZU*/ wire GUZU_FF1C_WRn = not1(HAGA_FF1C_WR);
    /*_p16.HUKY*/ ch3.HUKY_NR32_VOL0.dff9(GUZU_FF1C_WRn, GURO_APU_RESETn, BUS_CPU_D00p.out_old());
    /*_p16.HODY*/ ch3.HODY_NR32_VOL1.dff9(GUZU_FF1C_WRn, GURO_APU_RESETn, BUS_CPU_D01p.out_old());

    /*_p16.JOTU*/ wire JOTU_CPU_RDb = not1(AGUZ_CPU_RDn());
    /*_p16.HENU*/ wire HENU_FF1C_RD = nand2(GEFO_ADDR_FF1C, JOTU_CPU_RDb);

    /*_p18.JECO*/ wire JECO_DBG_CPU_RDb = not1(AGUZ_CPU_RDn());
    /*_p18.HONY*/ wire HONY_DBG_FF1C_RD = and3(EDEK_NR52_DBG_APUn, JECO_DBG_CPU_RDb, GEFO_ADDR_FF1C);
    /*_p18.GENO*/ wire GENO_DBG_FF1C_RDn = not1(HONY_DBG_FF1C_RD);

  #if 0
    if (HENU_FF1C_RD) set_data(
      /*_p18.FAPY*/ 0,
      /*_p18.FARO*/ 0,
      /*_p18.FOTE*/ 0,
      /*_p18.FANA*/ 0,
      /*_p18.FERA*/ 0,
      /*_p16.HAMU*/ HUKY_NR32_VOL0,
      /*_p16.HUCO*/ HODY_NR32_VOL1
    );
  #endif
  }

  //----------
  // FF1D - NR33 - Channel 3 Frequency's lower data (W)

  {
    /*_p10.EMOS*/ wire EMOS_ADDR_1101an = nand4(DOSO_A00p(), AFOB_A01n(), DENO_A02p(), DUCE_A03p());
    /*_p10.FENY*/ wire FENY_ADDR_FF1D  = nor2(BANU_ADDR_FF1Xn(), EMOS_ADDR_1101an);
    /*_p16.HOXA*/ wire HOXA_ADDR_FF1Dn = not1(FENY_ADDR_FF1D);

    /*_p16.KOTA*/ wire KOTA_FF1D_WRn = nand2(FENY_ADDR_FF1D, BOGY_CPU_WRp());
    /*_p16.JAFA*/ wire JAFA_FF1D_WRo = nand2(FENY_ADDR_FF1D, BOGY_CPU_WRp());

    /*_p16.KYHO*/ wire KYHO_FF1D_WRa = not1(KOTA_FF1D_WRn);
    /*_p16.KULY*/ wire KULY_FF1D_WRb = not1(JAFA_FF1D_WRo);
    /*_p16.KUHA*/ wire KUHA_APU_RESETq = not1(KEBA_APU_RSTp());

    /*_p16.KOGA*/ ch3.KOGA_NR33_FREQ00.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D00p.out_old());
    /*_p16.JOVY*/ ch3.JOVY_NR33_FREQ01.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D01p.out_old());
    /*_p16.JAXA*/ ch3.JAXA_NR33_FREQ02.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D02p.out_old());
    /*_p16.JEFE*/ ch3.JEFE_NR33_FREQ03.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D03p.out_old());
    /*_p16.JYPO*/ ch3.JYPO_NR33_FREQ04.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D04p.out_old());
    /*_p16.JOVE*/ ch3.JOVE_NR33_FREQ05.dff9(KYHO_FF1D_WRa, KUHA_APU_RESETq, BUS_CPU_D05p.out_old());
    /*_p16.KANA*/ ch3.KANA_NR33_FREQ06.dff9(KYHO_FF1D_WRa, KUHA_APU_RESETq, BUS_CPU_D06p.out_old());
    /*_p16.KOGU*/ ch3.KOGU_NR33_FREQ07.dff9(KYHO_FF1D_WRa, KUHA_APU_RESETq, BUS_CPU_D07p.out_old());

    /*_p16.GUTE*/ wire GUTE_DBG_FF1D_RDn = nor2(HOXA_ADDR_FF1Dn, EGAD_CPU_RDn_DBG);
    /*_p16.HOVO*/ wire HOVO_DBG_FF1D_RD = not1(GUTE_DBG_FF1D_RDn);

  #if 0
    if (HOVO_DBG_FF1D_RD) set_data(
      /*_p16.JOFO*/ KUTU_FREQ_00,
      /*_p16.KAFU*/ KUPE_FREQ_01,
      /*_p16.KESY*/ KUNU_FREQ_02,
      /*_p16.JUDE*/ KEMU_FREQ_03,
      /*_p16.JUKE*/ KYGU_FREQ_04,
      /*_p16.JEZA*/ KEPA_FREQ_05,
      /*_p16.KORA*/ KAFO_FREQ_06,
      /*_p16.KAMY*/ KENO_FREQ_07
    );
  #endif
  }

  //----------
  // FF1E - NR34 - Channel 3 Frequency's higher data (R/W)

  /*_p10.EGEN*/ wire EGEN_ADDR_1110n = nand4(DUCE_A03p(), DENO_A02p(), DUPA_A01p(), DYTE_A00n());
  /*_p10.DUGO*/ wire DUGO_ADDR_FF1Ep = nor2(BANU_ADDR_FF1Xn(), EGEN_ADDR_1110n);

  {
    /*#p16.GOMA*/ wire GOMA_APU_RESETn = not1(KEBA_APU_RSTp());
    /*#p16.FOBA*/ ch3.FOBA_CH3_TRIGp.dff17(DOVA_ABCDxxxx(), GOMA_APU_RESETn, ch3.GAVU_NR34_TRIG.qn_old());

    /*_p16.EPYX*/ wire EPYX_FF1E_WRp = nor2(BOGY_CPU_WRp(), DUGO_ADDR_FF1Ep); // polarity?
    /*_p16.FAKO*/ wire FAKO_RESTART_RST = nor2(KEBA_APU_RSTp(), ch3.FOBA_CH3_TRIGp.qp_new());
    /*_p16.GAVU*/ ch3.GAVU_NR34_TRIG.dff9   (EPYX_FF1E_WRp, FAKO_RESTART_RST, BUS_CPU_D07p.out_old());
  }

  {

    /*_p16.HUDA*/ wire HUDA_FF1E_WRn = nand2(DUGO_ADDR_FF1Ep, BOGY_CPU_WRp());

    /*_p16.KOPY*/ wire KOPY_APU_RESETn  = not1(KEBA_APU_RSTp());
    /*_p16.HEKY*/ wire HEKY_APU_RESETn  = not1(KEBA_APU_RSTp());

    /*_p16.JUZO*/ wire JUZO_FF1E_WR = not1(HUDA_FF1E_WRn);
    /*_p16.FOVO*/ wire FOVO_FF1E_WRo = nand2(ANUJ_CPU_WR_WEIRD, DUGO_ADDR_FF1Ep);

    /*_p16.JEMO*/ ch3.JEMO_NR34_FREQ08.dff9 (JUZO_FF1E_WR,  KOPY_APU_RESETn,  BUS_CPU_D00p.out_old());
    /*_p16.JETY*/ ch3.JETY_NR34_FREQ09.dff9 (JUZO_FF1E_WR,  KOPY_APU_RESETn,  BUS_CPU_D01p.out_old());
    /*_p16.JACY*/ ch3.JACY_NR34_FREQ10.dff9 (JUZO_FF1E_WR,  KOPY_APU_RESETn,  BUS_CPU_D02p.out_old());
    /*_p16.HOTO*/ ch3.HOTO_NR34_LENEN.dff9  (FOVO_FF1E_WRo, HEKY_APU_RESETn,  BUS_CPU_D06p.out_old());



    /*_p16.GUNU*/ wire GUNU = not1(DUGO_ADDR_FF1Ep);
    /*_p16.FUVA*/ wire FUVA_FF14_RDb = or2(GUNU, EGAD_CPU_RDn_DBG);

  #if 0
    if (FUVA_FF14_RDb) set_data(
      /*_p16.JUVY*/ KEJU_FREQ_08,
      /*_p16.JURA*/ KEZA_FREQ_09,
      /*_p16.HUFO*/ JAPU_FREQ_10,
      /*_p16.HACA*/ HOTO_NR34_STOP
    );
  #endif

    /*_p16.GORY*/ wire GORY = not1(AGUZ_CPU_RDn()); // polarity?
    /*_p16.GAWA*/ wire GAWA_FF14_RDa = nand2(DUGO_ADDR_FF1Ep, GORY); // polarity?

  #if 0
    if (GAWA_FF14_RDa) set_data(
      /*_p16.HACA*/ HOTO_NR34_STOP
    );
  #endif
  }

  //----------
  // Frequency counter

  {
    /*#p16.FURY*/ wire FURY_TRIG_RSTn = nor2(KEBA_APU_RSTp(), ch3.GYTA.qp_new());
    /*#p16.GULO*/ wire GULO_CH3_STOPp = not1(FURY_TRIG_RSTn);
    /*_p16.GOFY*/ ch3.GOFY_CH3_ACTIVEn.nor_latch(GULO_CH3_STOPp, ch3.FOBA_CH3_TRIGp.qp_new());

    /*#p16.FABO*/ wire FABO_CLK_xxCDxxGH = not1(spu.CERY_2M.qp_new());
    /*_p16.GAZE*/ wire GAZE_TRIG_RSTn = not1(KEBA_APU_RSTp());
    /*#p16.GARA*/ ch3.GARA.dff17(FABO_CLK_xxCDxxGH,    FURY_TRIG_RSTn, ch3.GOFY_CH3_ACTIVEn.qn_new());
    /*#p16.GYTA*/ ch3.GYTA.dff17(FABO_CLK_xxCDxxGH,    GAZE_TRIG_RSTn, ch3.GARA.qp_new());
    /*#p16.GYRA*/ ch3.GYRA.dff17(spu.CERY_2M.qp_new(), GAZE_TRIG_RSTn, ch3.GYTA.qp_new());
  }

  /*#p18.HUPA*/ wire HUPA = and2(ch3.HUNO_WAVE_CLKo.qp_new(), spu.CERY_2M.qp_new());

  /*#p18.HEMA*/ wire HEMA_WAVE_CLKb = not1(ch3.HUNO_WAVE_CLKo.qp_new());
  /*#p18.GASE*/ wire GASE_WAVE_CLKn = not1(HEMA_WAVE_CLKb);

  /*#p18.GAFU*/ wire GAFU = nor3(KEBA_APU_RSTp(), ch3.GARA.qp_new(), HUPA);

  /*#p18.HYFO*/ wire HYFO_CLK = not1(ch3.JAPU_FREQ_10.qp_new());
  /*#p18.HUNO*/ ch3.HUNO_WAVE_CLKo.dff17(HYFO_CLK, GAFU, ch3.HUNO_WAVE_CLKo.qn_old());

  /*_p16.FUVO*/ wire FUVO = nor2(ch3.GUXE_CH3_AMP_ENn.qp_new(), KEBA_APU_RSTp());
  /*#p16.FAJU*/ wire FAJU = not1(ch3.GYRA.qp_new());
  /*#p16.GUGU*/ ch3.GUGU_FREQ_CLK_STOP.nand_latch(FUVO, FAJU);

  {
    /*#p18.HERA*/ wire HERA_FREQ_RST  = nor2(GASE_WAVE_CLKn, ch3.GARA.qp_new());

    /*#p18.HEFO*/ wire HEFO_FREQ_CLKn = nor2(spu.CERY_2M.qp_new(), ch3.GUGU_FREQ_CLK_STOP.qp_new());
    /*_p18.JUTY*/ wire JUTY_FREQ_CLKa = not1(HEFO_FREQ_CLKn);
    /*_p18.KYKO*/ wire KYKO_FREQ_RSTn = not1(HERA_FREQ_RST);
    /*#p18.KUTU*/ ch3.KUTU_FREQ_00.dff20(JUTY_FREQ_CLKa,          KYKO_FREQ_RSTn, ch3.KOGA_NR33_FREQ00.qn_old());
    /*#p18.KUPE*/ ch3.KUPE_FREQ_01.dff20(ch3.KUTU_FREQ_00.qp_new(),   KYKO_FREQ_RSTn, ch3.JOVY_NR33_FREQ01.qn_old());
    /*_p18.KUNU*/ ch3.KUNU_FREQ_02.dff20(ch3.KUPE_FREQ_01.qp_new(),   KYKO_FREQ_RSTn, ch3.JAXA_NR33_FREQ02.qn_old());
    /*_p18.KEMU*/ ch3.KEMU_FREQ_03.dff20(ch3.KUNU_FREQ_02.qp_new(),   KYKO_FREQ_RSTn, ch3.JEFE_NR33_FREQ03.qn_old());

    /*#p18.KYRU*/ wire KYRU_FREQ_CLKb = not1(ch3.KEMU_FREQ_03.qn_new());
    /*_p18.JERA*/ wire JERA_FREQ_RSTo = not1(HERA_FREQ_RST);
    /*_p18.KYGU*/ ch3.KYGU_FREQ_04.dff20(KYRU_FREQ_CLKb,          JERA_FREQ_RSTo, ch3.JYPO_NR33_FREQ04.qn_old());
    /*_p18.KEPA*/ ch3.KEPA_FREQ_05.dff20(ch3.KYGU_FREQ_04.qp_new(),   JERA_FREQ_RSTo, ch3.JOVE_NR33_FREQ05.qn_old());
    /*_p18.KAFO*/ ch3.KAFO_FREQ_06.dff20(ch3.KEPA_FREQ_05.qp_new(),   JERA_FREQ_RSTo, ch3.KANA_NR33_FREQ06.qn_old());
    /*_p18.KENO*/ ch3.KENO_FREQ_07.dff20(ch3.KAFO_FREQ_06.qp_new(),   JERA_FREQ_RSTo, ch3.KOGU_NR33_FREQ07.qn_old());

    /*#p18.KESE*/ wire KESE_FREQ_CLKc = not1(ch3.KENO_FREQ_07.qn_new());
    /*_p18.KASO*/ wire KASO_FREQ_RSTp = not1(HERA_FREQ_RST);
    /*_p18.KEJU*/ ch3.KEJU_FREQ_08.dff20(KESE_FREQ_CLKc,          KASO_FREQ_RSTp, ch3.JEMO_NR34_FREQ08.qn_old());
    /*_p18.KEZA*/ ch3.KEZA_FREQ_09.dff20(ch3.KEJU_FREQ_08.qp_new(),   KASO_FREQ_RSTp, ch3.JETY_NR34_FREQ09.qn_old());
    /*_p18.JAPU*/ ch3.JAPU_FREQ_10.dff20(ch3.KEZA_FREQ_09.qp_new(),   KASO_FREQ_RSTp, ch3.JACY_NR34_FREQ10.qn_old());
  }

  {
    /*#p18.DERO*/ wire DERO_WAVE_CLK  = not1(GASE_WAVE_CLKn);
    /*#p18.ETAN*/ wire ETAN_WAVE_RST = or2(ch3.GARA.qp_new(), ch3.FETY_WAVE_LOOP.qp_new());

    /*#p18.EFAR*/ ch3.EFAR_WAVE_IDX0.dff17(DERO_WAVE_CLK,           ETAN_WAVE_RST, ch3.EFAR_WAVE_IDX0.qn_old());
    /*#p18.ERUS*/ ch3.ERUS_WAVE_IDX1.dff17(ch3.EFAR_WAVE_IDX0.qn_new(), ETAN_WAVE_RST, ch3.ERUS_WAVE_IDX1.qn_old());
    /*#p18.EFUZ*/ ch3.EFUZ_WAVE_IDX2.dff17(ch3.ERUS_WAVE_IDX1.qn_new(), ETAN_WAVE_RST, ch3.EFUZ_WAVE_IDX2.qn_old());
    /*#p18.EXEL*/ ch3.EXEL_WAVE_IDX3.dff17(ch3.EFUZ_WAVE_IDX2.qn_new(), ETAN_WAVE_RST, ch3.EXEL_WAVE_IDX3.qn_old());
    /*#p18.EFAL*/ ch3.EFAL_WAVE_IDX4.dff17(ch3.EXEL_WAVE_IDX3.qn_new(), ETAN_WAVE_RST, ch3.EFAL_WAVE_IDX4.qn_old());
   
    /*#p18.FOTO*/ wire FOTO = and2(ch3.FETY_WAVE_LOOP.qp_new(), GASE_WAVE_CLKn);
    /*#p18.GYRY*/ wire GYRY_LOOP_RST = nor3(KEBA_APU_RSTp(), ch3.GARA.qp_new(), FOTO);
    /*#p18.FETY*/ ch3.FETY_WAVE_LOOP.dff17(ch3.EFAL_WAVE_IDX4.qn_new(), GYRY_LOOP_RST, ch3.FETY_WAVE_LOOP.qn_old());
  }

  //----------
  // Wave ram control signals

  {  
    /*#p18.GEDO*/ wire GEDO = and2(ch3.FEXU_LEN_DONEp.qp_new(), ch3.HOTO_NR34_LENEN.qn_new()); // schematic had BUFY instead of FEXU?
    /*#p18.FYGO*/ wire FYGO = or3(KEBA_APU_RSTp(), GEDO, ch3.GUXE_CH3_AMP_ENn.qp_new());
    /*#p18.FOZU*/ ch3.FOZU.nor_latch(ch3.GARA.qp_new(), FYGO);
  }

  {
    /*#p18.EZAS*/ wire EZAS = not1(ch3.FOZU.qp_new());
    /*#p18.DORU*/ wire DORU = not1(EZAS);
    /*#p18.CALU*/ wire CALU_APU_RESETn = not1(KEBA_APU_RSTp());
    /*#p18.DAVO*/ ch3.DAVO_CH3_ACTIVEp.dff17(spu.AJER_2M.qp_new(), CALU_APU_RESETn, DORU);
  }











  // Wave RAM stuff
  /*#p10.AVUN*/   wire AVUN_A07n = not1(BUS_CPU_A07p.out_new());
  /*#p10.ASAD*/   wire ASAD_A06n = not1(BUS_CPU_A06p.out_new());
  /*#p10.ACOM*/   wire ACOM_ADDR_XX3Xn = nand4(AVUN_A07n, ASAD_A06n, BUS_CPU_A05p.out_new(), BUS_CPU_A04p.out_new());
  /*#p10.BARO*/ wire BARO_ADDR_FF3X  = nor2(ACOM_ADDR_XX3Xn, BAKO_ADDR_FFXXn());

  // ch3 sequencer something
  {
    
  
  
  }
  
  /*#p17.BOKE*/   wire BOKE_CPU_RDa = not1(AGUZ_CPU_RDn());
  /*#p17.BENA*/   wire BENA_CPU_WAVE_RDp = nand2(BOKE_CPU_RDa, BARO_ADDR_FF3X);
  /*#p17.CAZU*/ wire CAZU_CPU_WAVE_RDn = not1(BENA_CPU_WAVE_RDp);

  {
    // wave ram control line 1
    /*_p17.BYZA*/ wire BYZA_WAVE_WRp = and2(BOGY_CPU_WRp(), BARO_ADDR_FF3X);
    /*#p17.AMYT*/ wire AMYT_WAVE_WRn = not1(BYZA_WAVE_WRp); // to wave ram?

    // Wave read pulse generator and read reg clock
    /*#p17.ARUC*/ wire ARUC_CLK = not1(AMUK_xBxDxFxH());
    /*#p17.COZY*/ wire COZY_CLK = not1(AMUK_xBxDxFxH());
    /*#p17.BAMA*/ wire BAMA_APU_RESETn = not1(KEBA_APU_RSTp());
    /*#p17.BUSA*/ ch3.BUSA_WAVE_CLK_D1.dff17(AMUK_xBxDxFxH(), BAMA_APU_RESETn, GASE_WAVE_CLKn);
    /*#p17.BANO*/ ch3.BANO_WAVE_CLK_D2.dff17(COZY_CLK,        BAMA_APU_RESETn, ch3.BUSA_WAVE_CLK_D1.qp_old());
    /*#p17.AZUS*/ ch3.AZUS_WAVE_CLK_D3.dff17(AMUK_xBxDxFxH(), BAMA_APU_RESETn, ch3.BANO_WAVE_CLK_D2.qp_old());
    /*_p17.AZET*/ ch3.AZET_WAVE_CLK_D4.dff17(ARUC_CLK,        BAMA_APU_RESETn, ch3.AZUS_WAVE_CLK_D3.qp_old());

    /*#p17.BOXO*/ wire BOXO_TRIG = nor2(ch3.AZUS_WAVE_CLK_D3.qp_new(), ch3.AZET_WAVE_CLK_D4.qp_new()); // pulse generator
    /*#p17.BORU*/ wire BORU_TRIG = not1(BOXO_TRIG);

    wire BUKE_ABxxxxxH = 0;
    /*#p17.ABUR*/ wire ABUR_xxCDEFGx = not1(BUKE_ABxxxxxH);
    /*#p17.BORY*/ wire BORY_ABxxxxxH = not1(ABUR_xxCDEFGx);
    /*#p17.BETA*/ wire BETA_WAVE_RAM_CS = or3(BYZA_WAVE_WRp, CAZU_CPU_WAVE_RDn, BORY_ABxxxxxH);
    /*#p17.AZOR*/ wire AZOR_WAVE_RAM_CS = not1(BETA_WAVE_RAM_CS);
    /*#p17.BUKU*/ wire BUKU_WAVE_RAM_CS = not1(AZOR_WAVE_RAM_CS);

    /*#p17.ATUR*/ wire ATUR_WAVE_RAM_CS = mux2p(COKA_CH3_ACTIVEp, BORU_TRIG, BUKU_WAVE_RAM_CS);  // to wave ram? CS?
    /*#p17.ALER*/ wire ALER_WAVE_RAM_CS = not1(ATUR_WAVE_RAM_CS);
  }

  //----------
  // Wave ram address

  {
    /*_p18.BOLE*/ wire BOLE_WAVE_A0 = mux2p(COKA_CH3_ACTIVEp, ch3.ERUS_WAVE_IDX1.qp_new(), BUS_CPU_A00p.out_new());
    /*_p18.AGYL*/ wire AGYL_WAVE_A1 = mux2p(COKA_CH3_ACTIVEp, ch3.EFUZ_WAVE_IDX2.qp_new(), BUS_CPU_A01p.out_new());
    /*_p18.AFUM*/ wire AFUM_WAVE_A2 = mux2p(COKA_CH3_ACTIVEp, ch3.EXEL_WAVE_IDX3.qp_new(), BUS_CPU_A02p.out_new());
    /*_p18.AXOL*/ wire AXOL_WAVE_A3 = mux2p(COKA_CH3_ACTIVEp, ch3.EFAL_WAVE_IDX4.qp_new(), BUS_CPU_A03p.out_new());
  }

  // wave ram -> sample register
  {
    /*#p17.BUTU*/ wire BUTU_SAMPLE_CLK = not1(ch3.AZUS_WAVE_CLK_D3.qn_new());
    /*#p17.BEKA*/ wire BEKA_SAMPLE_CLKa = not1(BUTU_SAMPLE_CLK);
    /*#p17.COJU*/ wire COJU_SAMPLE_CLKb = not1(BUTU_SAMPLE_CLK);
    /*#p17.BAJA*/ wire BAJA_SAMPLE_CLKc = not1(BUTU_SAMPLE_CLK);
    /*#p17.BUFE*/ wire BUFE_SAMPLE_CLKd = not1(BUTU_SAMPLE_CLK);
    /*#p17.ACOR*/ wire ACOR_APU_RESETv = not1(KEBA_APU_RSTp());

    /*_p17.CYFO*/ ch3.CYFO_SAMPLE_0.dff9(BEKA_SAMPLE_CLKa, ACOR_APU_RESETv, BUS_WAVE_D00p.qp_old());
    /*_p17.CESY*/ ch3.CESY_SAMPLE_1.dff9(BUFE_SAMPLE_CLKd, ACOR_APU_RESETv, BUS_WAVE_D01p.qp_old());
    /*_p17.BUDY*/ ch3.BUDY_SAMPLE_2.dff9(BAJA_SAMPLE_CLKc, ACOR_APU_RESETv, BUS_WAVE_D02p.qp_old());
    /*_p17.BEGU*/ ch3.BEGU_SAMPLE_3.dff9(COJU_SAMPLE_CLKb, ACOR_APU_RESETv, BUS_WAVE_D03p.qp_old());
    /*_p17.CUVO*/ ch3.CUVO_SAMPLE_4.dff9(BEKA_SAMPLE_CLKa, ACOR_APU_RESETv, BUS_WAVE_D04p.qp_old());
    /*_p17.CEVO*/ ch3.CEVO_SAMPLE_5.dff9(BUFE_SAMPLE_CLKd, ACOR_APU_RESETv, BUS_WAVE_D05p.qp_old());
    /*_p17.BORA*/ ch3.BORA_SAMPLE_6.dff9(BAJA_SAMPLE_CLKc, ACOR_APU_RESETv, BUS_WAVE_D06p.qp_old());
    /*_p17.BEPA*/ ch3.BEPA_SAMPLE_7.dff9(COJU_SAMPLE_CLKb, ACOR_APU_RESETv, BUS_WAVE_D07p.qp_old());

    // wave ram control line 3
    /*#p18.BENO*/ wire BENO_WAVE_RAM_CTRL3n = mux2p(COKA_CH3_ACTIVEp, BUTU_SAMPLE_CLK, CAZU_CPU_WAVE_RDn); // to wave ram?
    /*#p18.ATOK*/ wire ATOK_WAVE_RAM_CTRL3p = not1(BENO_WAVE_RAM_CTRL3n); // to wave ram?
  }

  // wave ram -> cpu bus
  {
    /*#p17.CUGO*/ wire CUGO_WAVE_D0n = not1(BUS_WAVE_D00p.qp_new());
    /*#p17.CEGU*/ wire CEGU_WAVE_D1n = not1(BUS_WAVE_D01p.qp_new());
    /*#p17.ATEC*/ wire ATEC_WAVE_D2n = not1(BUS_WAVE_D02p.qp_new());
    /*#p17.ADOK*/ wire ADOK_WAVE_D3n = not1(BUS_WAVE_D03p.qp_new());
    /*#p17.BACA*/ wire BACA_WAVE_D4n = not1(BUS_WAVE_D04p.qp_new());
    /*#p17.BERO*/ wire BERO_WAVE_D5n = not1(BUS_WAVE_D05p.qp_new());
    /*#p17.CUTO*/ wire CUTO_WAVE_D6n = not1(BUS_WAVE_D06p.qp_new());
    /*#p17.AKAF*/ wire AKAF_WAVE_D7n = not1(BUS_WAVE_D07p.qp_new());

    /*#p17.DUGU*/ triwire DUGU = tri6_pn(CAZU_CPU_WAVE_RDn, CUGO_WAVE_D0n);
    /*#p17.DESY*/ triwire DESY = tri6_pn(CAZU_CPU_WAVE_RDn, CEGU_WAVE_D1n);
    /*#p17.BATY*/ triwire BATY = tri6_pn(CAZU_CPU_WAVE_RDn, ATEC_WAVE_D2n);
    /*#p17.BADE*/ triwire BADE = tri6_pn(CAZU_CPU_WAVE_RDn, ADOK_WAVE_D3n);
    /*#p17.BUNE*/ triwire BUNE = tri6_pn(CAZU_CPU_WAVE_RDn, BACA_WAVE_D4n);
    /*#p17.BAVA*/ triwire BAVA = tri6_pn(CAZU_CPU_WAVE_RDn, BERO_WAVE_D5n);
    /*#p17.DESA*/ triwire DESA = tri6_pn(CAZU_CPU_WAVE_RDn, CUTO_WAVE_D6n);
    /*#p17.BEZU*/ triwire BEZU = tri6_pn(CAZU_CPU_WAVE_RDn, AKAF_WAVE_D7n);

    BUS_CPU_D00p.tri_bus(DUGU);
    BUS_CPU_D01p.tri_bus(DESY);
    BUS_CPU_D02p.tri_bus(BATY);
    BUS_CPU_D03p.tri_bus(BADE);
    BUS_CPU_D04p.tri_bus(BUNE);
    BUS_CPU_D05p.tri_bus(BAVA);
    BUS_CPU_D06p.tri_bus(DESA);
    BUS_CPU_D07p.tri_bus(BEZU);
  }

  //----------
  // Output

  {
    /*#p17.DATE*/ wire DATE_WAVE_PLAY_D0 = mux2n(ch3.EFAR_WAVE_IDX0.qp_new(), ch3.CYFO_SAMPLE_0.qp_new(), ch3.CUVO_SAMPLE_4.qp_new());
    /*#p17.DAZY*/ wire DAZY_WAVE_PLAY_D1 = mux2n(ch3.EFAR_WAVE_IDX0.qp_new(), ch3.CESY_SAMPLE_1.qp_new(), ch3.CEVO_SAMPLE_5.qp_new());
    /*#p17.CUZO*/ wire CUZO_WAVE_PLAY_D2 = mux2n(ch3.EFAR_WAVE_IDX0.qp_new(), ch3.BUDY_SAMPLE_2.qp_new(), ch3.BORA_SAMPLE_6.qp_new());
    /*#p17.COPO*/ wire COPO_WAVE_PLAY_D3 = mux2n(ch3.EFAR_WAVE_IDX0.qp_new(), ch3.BEGU_SAMPLE_3.qp_new(), ch3.BEPA_SAMPLE_7.qp_new());

#if 0
    // Volume numbering seems wrong...
    if (vol == 0) {
      /*#p18.EZAG*/ wire EZAG_WAVE_OUT0 = CUZO_WAVE_PLAY_D2;
      /*#p18.EVUG*/ wire EVUG_WAVE_OUT1 = COPO_WAVE_PLAY_D3;
      /*#p18.DOKY*/ wire DOKY_WAVE_OUT2 = 0;
      /*#p18.DORE*/ wire DORE_WAVE_OUT3 = 0;
    }
    else if (vol == 1) {
      /*#p18.EZAG*/ wire EZAG_WAVE_OUT0 = DATE_WAVE_PLAY_D0;
      /*#p18.EVUG*/ wire EVUG_WAVE_OUT1 = DAZY_WAVE_PLAY_D1;
      /*#p18.DOKY*/ wire DOKY_WAVE_OUT2 = CUZO_WAVE_PLAY_D2;
      /*#p18.DORE*/ wire DORE_WAVE_OUT3 = COPO_WAVE_PLAY_D3;
    }
    else if (vol == 2) {
      /*#p18.EZAG*/ wire EZAG_WAVE_OUT0 = DAZY_WAVE_PLAY_D1;
      /*#p18.EVUG*/ wire EVUG_WAVE_OUT1 = CUZO_WAVE_PLAY_D2;
      /*#p18.DOKY*/ wire DOKY_WAVE_OUT2 = COPO_WAVE_PLAY_D3;
      /*#p18.DORE*/ wire DORE_WAVE_OUT3 = 0;
    }
    else if (vol == 3) {
      /*#p18.EZAG*/ wire EZAG_WAVE_OUT0 = 0;
      /*#p18.EVUG*/ wire EVUG_WAVE_OUT1 = 0;
      /*#p18.DOKY*/ wire DOKY_WAVE_OUT2 = 0;
      /*#p18.DORE*/ wire DORE_WAVE_OUT3 = 0;
    }

#endif

    /*#p18.GEMY*/ wire GEMY_WAVE_VOL3 = nor2(ch3.HUKY_NR32_VOL0.qp_new(), ch3.HODY_NR32_VOL1.qp_new());
    /*#p18.GOKA*/ wire GOKA_WAVE_VOL2 = nor2(ch3.HUKY_NR32_VOL0.qp_new(), ch3.HODY_NR32_VOL1.qn_new());
    /*#p18.GEGE*/ wire GEGE_WAVE_VOL4 = nor2(ch3.HUKY_NR32_VOL0.qn_new(), ch3.HODY_NR32_VOL1.qp_new());

    /*#p18.EZAG*/ wire EZAG_WAVE_OUT0 = amux3(DATE_WAVE_PLAY_D0, GEGE_WAVE_VOL4, DAZY_WAVE_PLAY_D1, GOKA_WAVE_VOL2, CUZO_WAVE_PLAY_D2, GEMY_WAVE_VOL3);
    /*#p18.EVUG*/ wire EVUG_WAVE_OUT1 = amux3(DAZY_WAVE_PLAY_D1, GEGE_WAVE_VOL4, CUZO_WAVE_PLAY_D2, GOKA_WAVE_VOL2, COPO_WAVE_PLAY_D3, GEMY_WAVE_VOL3);
    /*#p18.DOKY*/ wire DOKY_WAVE_OUT2 = amux2(CUZO_WAVE_PLAY_D2, GEGE_WAVE_VOL4, COPO_WAVE_PLAY_D3, GOKA_WAVE_VOL2);
    /*#p18.DORE*/ wire DORE_WAVE_OUT3 = and2 (COPO_WAVE_PLAY_D3, GEGE_WAVE_VOL4);

    // these go straight to the dac
    /*#p18.BARY*/ wire BARY_WAVE_DAC0 = and2(COKA_CH3_ACTIVEp, EZAG_WAVE_OUT0);
    /*#p18.BYKA*/ wire BYKA_WAVE_DAC1 = and2(COKA_CH3_ACTIVEp, EVUG_WAVE_OUT1);
    /*#p18.BOPA*/ wire BOPA_WAVE_DAC2 = and2(COKA_CH3_ACTIVEp, DOKY_WAVE_OUT2);
    /*#p18.BELY*/ wire BELY_WAVE_DAC3 = and2(COKA_CH3_ACTIVEp, DORE_WAVE_OUT3);
  }





















































  //========================================
  //========================================
  //========================================
  //========================================
  //             CHANNEL 4
  //========================================
  //========================================
  //========================================
  //========================================

  /*#p09.DAPA*/ wire DAPA_RSTn = not1(KEBA_APU_RSTp()); // bavu crosses over this? why is it 3-rung?


  //----------
  // FF20 NR41 - the length register is also the length timer

  {
    /*#p10.DONA*/ wire DONA_ADDR_0000bn = nand4(DYTE_A00n(), AFOB_A01n(), ABUB_A02n(), ACOL_A03n());
    /*#p10.DANU*/ wire DANU_FF20a    = nor2(DONA_ADDR_0000bn, BEZY_ADDR_FF2Xn());
    /*#p19.CAZE*/ wire CAZE_FF20_WRn = nand2(BOGY_CPU_WRp(), DANU_FF20a);
    /*#p19.FURU*/ wire FURU_FF20_WRa = not1(CAZE_FF20_WRn);
    /*#p19.DOTU*/ wire DOTU_FF20_WRb = not1(CAZE_FF20_WRn);
    /*#p19.EPEK*/ wire EPEK_FF20_WRc = not1(CAZE_FF20_WRn);

    /*#p19.GAPY*/ wire GAPY_LEN_RSTn = nor3(FURU_FF20_WRa, KEBA_APU_RSTp(), ch4.GONE_CH4_TRIGp.qp_new());

    /*#p19.DODA*/ wire DODA_LEN_CLKn = nor3(ch4.FUGO_CH4_LEN_DONEp.qp_new(), BUFY_CLK_256n(), ch4.CUNY_NR44_LEN_ENp.qp_new());
    /*#p19.CUWA*/ wire CUWA_LEN_CLKa = not1(DODA_LEN_CLKn);

    /*#p19.DANO*/ ch4.DANO_NR41_LEN0.dff20(CUWA_LEN_CLKa,   DOTU_FF20_WRb, BUS_CPU_D00p.qp_old());
    /*#p19.FAVY*/ ch4.FAVY_NR41_LEN1.dff20(ch4.DANO_NR41_LEN0.qp_new(),   DOTU_FF20_WRb, BUS_CPU_D01p.qp_old());
    /*#p19.DENA*/ ch4.DENA_NR41_LEN2.dff20(ch4.FAVY_NR41_LEN1.qp_new(),   DOTU_FF20_WRb, BUS_CPU_D02p.qp_old());
    /*#p19.CEDO*/ ch4.CEDO_NR41_LEN3.dff20(ch4.DENA_NR41_LEN2.qp_new(),   DOTU_FF20_WRb, BUS_CPU_D03p.qp_old());

    /*#p19.DOPU*/ wire DOPU_NR41_LEN3p = not1(ch4.CEDO_NR41_LEN3.qn_new());
    /*#p19.FYLO*/ ch4.FYLO_NR41_LEN4.dff20(DOPU_NR41_LEN3p, EPEK_FF20_WRc, BUS_CPU_D04p.qp_old());
    /*#p19.EDOP*/ ch4.EDOP_NR41_LEN5.dff20(ch4.FYLO_NR41_LEN4.qp_new(),   EPEK_FF20_WRc, BUS_CPU_D05p.qp_old());

    /*#p19.FUGO*/ ch4.FUGO_CH4_LEN_DONEp.dff17(ch4.EDOP_NR41_LEN5.qn_new(), GAPY_LEN_RSTn, ch4.FUGO_CH4_LEN_DONEp.qn_old());
  }

  //----------
  // FF21
  
  {
    /*#p10.DEWA*/ wire DEWA_ADDR_0001bn = nand4(DOSO_A00p(), AFOB_A01n(), ABUB_A02n(), ACOL_A03n());
    /*#p10.COVO*/ wire COVO_FF21p    = nor2(DEWA_ADDR_0001bn, BEZY_ADDR_FF2Xn());
    /*#p19.BOFY*/ wire BOFY_FF21n    = not1(COVO_FF21p);
    /*#p19.GONY*/ wire GONY_FF21n    = not1(COVO_FF21p);
    /*#p19.DACO*/ wire DACO_FF21_WRp = and2(BOGY_CPU_WRp(), COVO_FF21p);
    /*#p19.GOKO*/ wire GOKO_FF21_WRp = and2(COVO_FF21p, BOGY_CPU_WRp());
    /*#p19.BOXE*/ wire BOXE_FF21_RDn = or2(BOFY_FF21n, AGUZ_CPU_RDn());
    /*#p19.HASU*/ wire HASU_FF21_RDn = or2(GONY_FF21n, AGUZ_CPU_RDn());

    /*#p19.FEXO*/ wire FEXO_RSTn = not1(KEBA_APU_RSTp());

    /*#p19.DYKE*/ wire DYKE_FF21_WRn = not1(DACO_FF21_WRp);
    /*#p19.FUPA*/ wire FUPA_FF21_WRn = not1(GOKO_FF21_WRp);
    
    /*_p19.EMOK*/ ch4.EMOK_NR42_ENV_TIMER0 .dff9(DYKE_FF21_WRn, FEXO_RSTn, BUS_CPU_D00p.qp_old());
    /*_p19.ETYJ*/ ch4.ETYJ_NR42_ENV_TIMER1 .dff9(DYKE_FF21_WRn, FEXO_RSTn, BUS_CPU_D01p.qp_old());
    /*_p19.EZYK*/ ch4.EZYK_NR42_ENV_TIMER2 .dff9(DYKE_FF21_WRn, FEXO_RSTn, BUS_CPU_D02p.qp_old());
    /*_p19.GEKY*/ ch4.GEKY_NR42_ENV_DIR    .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D03p.qp_old());
    /*_p19.GARU*/ ch4.GARU_NR42_ENV0   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D04p.qp_old());
    /*_p19.GOKY*/ ch4.GOKY_NR42_ENV1   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D05p.qp_old());
    /*_p19.GOZO*/ ch4.GOZO_NR42_ENV2   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D06p.qp_old());
    /*_p19.GEDU*/ ch4.GEDU_NR42_ENV3   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D07p.qp_old());

    /*#p19.DEMY*/ triwire DEMY = tri6_nn(BOXE_FF21_RDn, ch4.EMOK_NR42_ENV_TIMER0.qp_new());
    /*#p19.COCE*/ triwire COCE = tri6_nn(BOXE_FF21_RDn, ch4.ETYJ_NR42_ENV_TIMER1.qp_new());
    /*#p19.CUZU*/ triwire CUZU = tri6_nn(BOXE_FF21_RDn, ch4.EZYK_NR42_ENV_TIMER2.qp_new());
    /*#p19.GOME*/ triwire GOME = tri6_nn(HASU_FF21_RDn, ch4.GEKY_NR42_ENV_DIR.qp_new());
    /*#p19.HEDA*/ triwire HEDA = tri6_nn(HASU_FF21_RDn, ch4.GARU_NR42_ENV0.qp_new());
    /*#p19.GODU*/ triwire GODU = tri6_nn(HASU_FF21_RDn, ch4.GOKY_NR42_ENV1.qp_new());
    /*#p19.HOGE*/ triwire HOGE = tri6_nn(HASU_FF21_RDn, ch4.GOZO_NR42_ENV2.qp_new());
    /*#p19.HACU*/ triwire HACU = tri6_nn(HASU_FF21_RDn, ch4.GEDU_NR42_ENV3.qp_new());
  }

  //----------
  // FF22

  {
    /*#p10.DOFA*/ wire DOFA_ADDR_0010p = and4(ACOL_A03n(), ABUB_A02n(), DUPA_A01p(), DYTE_A00n());
    /*#p10.EKEZ*/ wire EKEZ_FF22p = and2(CONA_ADDR_FF2Xp(), DOFA_ADDR_0010p);
    /*#p19.KOKU*/ wire KOKU_FF22n = not1(EKEZ_FF22p);
    /*#p19.GUGO*/ wire GUGO_FF22n = not1(EKEZ_FF22p);

    /*#p19.HUMO*/ wire HUMO_FF22_WRp = and2(BOGY_CPU_WRp(), EKEZ_FF22p);
    /*#p19.GETU*/ wire GETU_FF22_WRp = and2(BOGY_CPU_WRp(), EKEZ_FF22p);

    /*#p19.KEKA*/ wire KEKA_FF22_RDn = or2(KOKU_FF22n, AGUZ_CPU_RDn());

    /*#p19.KAGE*/ wire KAGE_CPU_RDp = not1(AGUZ_CPU_RDn());
    /*#p19.JORA*/ wire JORA_FF22_RDn = nand2(KAGE_CPU_RDp, EKEZ_FF22p);
    /*#p19.HEZE*/ wire HEZE_FF22_RDn = or2(GUGO_FF22n, AGUZ_CPU_RDn());

    /*#p09.KAME*/ wire KAME_RSTn = not1(KEBA_APU_RSTp());
    /*#p19.HYNE*/ wire HYNE_RSTn = not1(KEBA_APU_RSTp());

    /*#p19.HOVA*/ wire HOVA_FF22_WRn = not1(HUMO_FF22_WRp);
    /*#p19.HOSO*/ wire HOSO_FF22_WRn = nand2(EKEZ_FF22p, BOGY_CPU_WRp());
    /*#p19.EFUG*/ wire EFUG_FF22_WRn = not1(GETU_FF22_WRp);
    /*#p19.JARE*/ ch4.JARE_NR43_DIV0  .dff9(HOVA_FF22_WRn, KAME_APU_RSTn(), BUS_CPU_D00p.qp_new());
    /*#p19.JERO*/ ch4.JERO_NR43_DIV1  .dff9(HOVA_FF22_WRn, KAME_APU_RSTn(), BUS_CPU_D01p.qp_new());
    /*#p19.JAKY*/ ch4.JAKY_NR43_DIV2  .dff9(HOVA_FF22_WRn, KAME_APU_RSTn(), BUS_CPU_D02p.qp_new());
    /*#p19.JAMY*/ ch4.JAMY_NR43_MODE  .dff9(HOSO_FF22_WRn, HYNE_RSTn,       BUS_CPU_D03p.qp_new());
    /*#p19.FETA*/ ch4.FETA_NR43_FREQ0 .dff9(EFUG_FF22_WRn, DAPA_RSTn,       BUS_CPU_D04p.qp_new());
    /*#p19.FYTO*/ ch4.FYTO_NR43_FREQ1 .dff9(EFUG_FF22_WRn, DAPA_RSTn,       BUS_CPU_D05p.qp_new());
    /*_p19.GOGO*/ ch4.GOGO_NR43_FREQ2 .dff9(EFUG_FF22_WRn, DAPA_RSTn,       BUS_CPU_D06p.qp_new());
    /*_p19.GAFO*/ ch4.GAFO_NR43_FREQ3 .dff9(EFUG_FF22_WRn, DAPA_RSTn,       BUS_CPU_D07p.qp_new());

    /*#p19.KAMO*/ triwire KAMO = tri6_nn(KEKA_FF22_RDn, ch4.JARE_NR43_DIV0.qp_new());
    /*#p19.KAKU*/ triwire KAKU = tri6_nn(KEKA_FF22_RDn, ch4.JERO_NR43_DIV1.qp_new());
    /*_p19.KYRO*/ triwire KYRO = tri6_nn(KEKA_FF22_RDn, ch4.JAKY_NR43_DIV2.qp_new());
    /*_p19.KETA*/ triwire KETA = tri6_nn(JORA_FF22_RDn, ch4.JAMY_NR43_MODE.qp_new());
    /*_p19.GEDA*/ triwire GEDA = tri6_nn(HEZE_FF22_RDn, ch4.FETA_NR43_FREQ0.qp_new());
    /*_p19.GYPE*/ triwire GYPE = tri6_nn(HEZE_FF22_RDn, ch4.FYTO_NR43_FREQ1.qp_new());
    /*_p19.GAKA*/ triwire GAKA = tri6_nn(HEZE_FF22_RDn, ch4.GOGO_NR43_FREQ2.qp_new());
    /*_p19.HAPY*/ triwire HAPY = tri6_nn(HEZE_FF22_RDn, ch4.GAFO_NR43_FREQ3.qp_new());
  }

  //----------
  // FF23. Some weird debug voodoo here.

  /*#p20.DYRY*/ wire DYRY_DBG = and2(ch4.CUNY_NR44_LEN_ENp.qn_new(), EDEK_NR52_DBG_APUn);

  {
    /*#p19.BYLO*/ wire BYLO_CPU_RDp = not1(AGUZ_CPU_RDn());
    /*#p10.DUFE*/ wire DUFE_ADDR_0011n = nand4(DOSO_A00p(), DUPA_A01p(), ABUB_A02n(), ACOL_A03n());
    /*#p10.CUGE*/ wire CUGE_FF23p    = nor2(DUFE_ADDR_0011n, BEZY_ADDR_FF2Xn());
    /*#p19.BARE*/ wire BARE_FF23_RDn = nand2(CUGE_FF23p, BYLO_CPU_RDp);

    /*#p19.CABE*/ wire CABE_RSTn = not1(KEBA_APU_RSTp());

    /*#p19.DULU*/ wire DULU_FF23_WRn = nand2(ANUJ_CPU_WR_WEIRD, CUGE_FF23p);
    /*#p19.CUNY*/ ch4.CUNY_NR44_LEN_ENp.dff9(DULU_FF23_WRn, CABE_RSTn, BUS_CPU_D06p.qp_new());

    /*#p19.FOXE*/ wire FOXE_FF23_WRn = nand2(BOGY_CPU_WRp(), CUGE_FF23p);
    /*#p20.GUZY*/ wire GUZY_NR44_TRIG_RST = nor2(KEBA_APU_RSTp(), ch4.GYSU_CH4_TRIG.qp_new());
    /*#p19.HOGA*/ ch4.HOGA_NR44_TRIG.dff9(FOXE_FF23_WRn, GUZY_NR44_TRIG_RST, BUS_CPU_D07p.qp_old());

    /*#p19.CURY*/ triwire CURY = tri6_nn(BARE_FF23_RDn, ch4.CUNY_NR44_LEN_ENp.qp_new());

    //----------
    // Debug

    /*#p20.COSA*/ wire COSA_CPU_RDp = not1(AGUZ_CPU_RDn());
    /*_p20.COMO*/ wire COMO_DBG = and2(DYRY_DBG, COSA_CPU_RDp);
    /*_p20.BAGU*/ wire BAGU_DBG = nand2(CUGE_FF23p, COMO_DBG);
    ///*_p20.BEFA*/ wire BEFA_DBG = not1(CARY);

    /*_p20.ATEL*/ triwire ATEL = tri6_nn(BAGU_DBG, BEZY_ADDR_FF2Xn());
  }


  //----------
  // Control

  /*#p20.GEVY*/ wire GEVY_CH4_AMP_ENn = nor5(ch4.GEKY_NR42_ENV_DIR.qn_new(),
                                             ch4.GARU_NR42_ENV0.qn_new(),
                                             ch4.GOKY_NR42_ENV1.qn_new(),
                                             ch4.GOZO_NR42_ENV2.qn_new(),
                                             ch4.GEDU_NR42_ENV3.qn_new());

  {
    /*#p20.GASO*/ wire GASO_RSTn = not1(KEBA_APU_RSTp());
    /*#p20.GYSU*/ ch4.GYSU_CH4_TRIG.dff17(DOVA_ABCDxxxx(), GASO_RSTn, ch4.HOGA_NR44_TRIG.qn_old());
  }

  {
    /*#p20.EFOT*/ wire EFOT_CH4_STOPp = and2(ch4.CUNY_NR44_LEN_ENp.qn_new(), ch4.FUGO_CH4_LEN_DONEp.qp_new());
    /*#p20.FEGY*/ wire FEGY_CH4_STOPp = or3(KEBA_APU_RSTp(), EFOT_CH4_STOPp, GEVY_CH4_AMP_ENn);
    /*#p20.GENA*/ ch4.GENA_CH4_ACTIVEp.nor_latch(ch4.GONE_CH4_TRIGp.qp_new(), FEGY_CH4_STOPp);
  }

  {
    /*#p20.FALE*/ wire FALE_RESET_LATCHp = nor2(KEBA_APU_RSTp(), ch4.GORA_CH4_TRIGp.qp_new());
    /*#p20.HELU*/ wire HELU_RESET_LATCHp  = not1(FALE_RESET_LATCHp);
    /*#p20.HAZO*/ ch4.HAZO_CH4_TRIGn.nor_latch(HELU_RESET_LATCHp, ch4.GYSU_CH4_TRIG.qp_new());

    /*#p20.FEBY*/ wire FEBY_RSTn = not1(KEBA_APU_RSTp());
    /*#p20.GONE*/ ch4.GONE_CH4_TRIGp.dff17(HAMA_CLK_512K(), FALE_RESET_LATCHp, ch4.HAZO_CH4_TRIGn.qn_new());
    /*#p20.GORA*/ ch4.GORA_CH4_TRIGp.dff17(HAMA_CLK_512K(), FEBY_RSTn,         ch4.GONE_CH4_TRIGp.qp_old());
    /*#p20.GATY*/ ch4.GATY_CH4_TRIGp.dff17(HAMA_CLK_512K(), FEBY_RSTn,         ch4.GORA_CH4_TRIGp.qp_old());
  }

  {
    /*#p20.HERY*/ wire HERY_DIV_GATE1n = nor2(GEVY_CH4_AMP_ENn, KEBA_APU_RSTp());
    /*#p20.HAPU*/ wire HAPU_CH4_TRIGn = not1(ch4.GATY_CH4_TRIGp.qp_new());
    /*#p20.JERY*/ ch4.JERY_DIV_GATE.nand_latch(HERY_DIV_GATE1n, HAPU_CH4_TRIGn);
  }

  {
    /*#p20.KYKU*/ wire KYKU_DIV_CLKb  = or2(ch4.JERY_DIV_GATE.qp_new(), spu.JESO_CLK_512K.qp_new());
    /*#p20.KONY*/ wire KONY_DIV_CLKn  = not1(KYKU_DIV_CLKb);
    /*#p20.KANU*/ wire KANU_DIV_CLKa  = not1(KONY_DIV_CLKn);

    /*#p20.GOFU*/ wire GOFU_DIV_LOADn = nor2(ch4.GONE_CH4_TRIGp.qp_new(), ch4.GARY_FREQ_GATEp.qp_new());
    /*#p20.HUCE*/ wire HUCE_DIV_LOAD  = not1(GOFU_DIV_LOADn);

    /*#p20.JYCO*/ ch4.JYCO_DIV0.dff20(KANU_DIV_CLKa,          HUCE_DIV_LOAD, ch4.JARE_NR43_DIV0.qp_old());
    /*#p20.JYRE*/ ch4.JYRE_DIV1.dff20(ch4.JYCO_DIV0.qp_new(), HUCE_DIV_LOAD, ch4.JERO_NR43_DIV1.qp_old());
    /*_p20.JYFU*/ ch4.JYFU_DIV2.dff20(ch4.JYRE_DIV1.qp_new(), HUCE_DIV_LOAD, ch4.JAKY_NR43_DIV2.qp_old());
  }

  {
    /*#p20.GYBA*/ wire GYBA_1M = not1(BAVU_1M);
    /*#p20.GUNY*/ wire GUNY_FREQ_GATE_RSTn = nor2(KEBA_APU_RSTp(), ch4.GONE_CH4_TRIGp.qp_new());
    /*#p20.HYNO*/ wire HYNO_DIV_MAX = or3(ch4.JYFU_DIV2.qp_new(), ch4.JYRE_DIV1.qp_new(), ch4.JYCO_DIV0.qp_new());
    /*#p20.GARY*/ ch4.GARY_FREQ_GATEp.dff17(GYBA_1M, GUNY_FREQ_GATE_RSTn, HYNO_DIV_MAX);
  }

  {

    /*#p20.CARY*/ wire CARY_FREQ_CLK = and2(BAVU_1M, ch4.GARY_FREQ_GATEp.qp_new());
    /*#p20.CEXO*/ ch4.CEXO_FREQ_00.dff17(CARY_FREQ_CLK,             DAPA_RSTn, ch4.CEXO_FREQ_00.qn_old());
    /*_p20.DEKO*/ ch4.DEKO_FREQ_01.dff17(ch4.CEXO_FREQ_00.qn_new(), DAPA_RSTn, ch4.DEKO_FREQ_01.qn_old());
    /*_p20.EZEF*/ ch4.EZEF_FREQ_02.dff17(ch4.DEKO_FREQ_01.qn_new(), DAPA_RSTn, ch4.EZEF_FREQ_02.qn_old());
    /*_p20.EPOR*/ ch4.EPOR_FREQ_03.dff17(ch4.EZEF_FREQ_02.qn_new(), DAPA_RSTn, ch4.EPOR_FREQ_03.qn_old());
    /*_p20.DURE*/ ch4.DURE_FREQ_04.dff17(ch4.EPOR_FREQ_03.qn_new(), DAPA_RSTn, ch4.DURE_FREQ_04.qn_old());
    /*_p20.DALE*/ ch4.DALE_FREQ_05.dff17(ch4.DURE_FREQ_04.qn_new(), DAPA_RSTn, ch4.DALE_FREQ_05.qn_old());
    /*_p20.DOKE*/ ch4.DOKE_FREQ_06.dff17(ch4.DALE_FREQ_05.qn_new(), DAPA_RSTn, ch4.DOKE_FREQ_06.qn_old());
    /*_p20.DEMO*/ ch4.DEMO_FREQ_07.dff17(ch4.DOKE_FREQ_06.qn_new(), DAPA_RSTn, ch4.DEMO_FREQ_07.qn_old());
    /*_p20.DOSE*/ ch4.DOSE_FREQ_08.dff17(ch4.DEMO_FREQ_07.qn_new(), DAPA_RSTn, ch4.DOSE_FREQ_08.qn_old());
    /*_p20.DETE*/ ch4.DETE_FREQ_09.dff17(ch4.DOSE_FREQ_08.qn_new(), DAPA_RSTn, ch4.DETE_FREQ_09.qn_old());
    /*_p20.ERUT*/ ch4.ERUT_FREQ_10.dff17(ch4.DETE_FREQ_09.qn_new(), DAPA_RSTn, ch4.ERUT_FREQ_10.qn_old());
    /*_p20.DOTA*/ ch4.DOTA_FREQ_11.dff17(ch4.ERUT_FREQ_10.qn_new(), DAPA_RSTn, ch4.DOTA_FREQ_11.qn_old());
    /*_p20.DERE*/ ch4.DERE_FREQ_12.dff17(ch4.DOTA_FREQ_11.qn_new(), DAPA_RSTn, ch4.DERE_FREQ_12.qn_old());
    /*_p20.ESEP*/ ch4.ESEP_FREQ_13.dff17(ch4.DERE_FREQ_12.qn_new(), DAPA_RSTn, ch4.ESEP_FREQ_13.qn_old());
  }

  //----------
  // The actual LFSR

  // 14-to-1 mux to select LFSR clock

  // FIXME did you just copy-paste this block and not finish it?

  /*#p20.FEME*/ bool FEME_LFSR_CLKp;

  {
    /*_p20.EMOF*/ wire EMOF_LFSR_CLK_MUX_7 = nor3(ch4.FETA_NR43_FREQ0.qn_new(), ch4.FYTO_NR43_FREQ1.qn_new(), ch4.GOGO_NR43_FREQ2.qn_new());
    /*_p20.ELAR*/ wire ELAR_LFSR_CLK_MUX_6 = nor3(ch4.FETA_NR43_FREQ0.qn_new(), ch4.FYTO_NR43_FREQ1.qn_new(), ch4.GOGO_NR43_FREQ2.qp_new());
    /*_p20.DUDU*/ wire DUDU_LFSR_CLK_MUX_5 = nor3(ch4.FETA_NR43_FREQ0.qn_new(), ch4.FYTO_NR43_FREQ1.qp_new(), ch4.GOGO_NR43_FREQ2.qn_new());
    /*_p20.ETAT*/ wire ETAT_LFSR_CLK_MUX_4 = nor3(ch4.FETA_NR43_FREQ0.qn_new(), ch4.FYTO_NR43_FREQ1.qp_new(), ch4.GOGO_NR43_FREQ2.qp_new());
    /*_p20.FURA*/ wire FURA_LFSR_CLK_MUX_3 = nor3(ch4.FETA_NR43_FREQ0.qp_new(), ch4.FYTO_NR43_FREQ1.qn_new(), ch4.GOGO_NR43_FREQ2.qn_new());
    /*_p20.ETAR*/ wire ETAR_LFSR_CLK_MUX_2 = nor3(ch4.FETA_NR43_FREQ0.qp_new(), ch4.FYTO_NR43_FREQ1.qn_new(), ch4.GOGO_NR43_FREQ2.qp_new());
    /*_p20.EVER*/ wire EVER_LFSR_CLK_MUX_1 = nor3(ch4.FETA_NR43_FREQ0.qp_new(), ch4.FYTO_NR43_FREQ1.qp_new(), ch4.GOGO_NR43_FREQ2.qn_new());
    /*_p20.ETOV*/ wire ETOV_LFSR_CLK_MUX_0 = nor3(ch4.FETA_NR43_FREQ0.qp_new(), ch4.FYTO_NR43_FREQ1.qp_new(), ch4.GOGO_NR43_FREQ2.qp_new());

    /*_p20.ETYR*/ wire ETYR_LFSR_CLK_MUX_A = amux6(ch4.ESEP_FREQ_13.qp_new(), DUDU_LFSR_CLK_MUX_5,
                                                  ch4.DERE_FREQ_12.qp_new(), ETAT_LFSR_CLK_MUX_4,
                                                  ch4.DOTA_FREQ_11.qp_new(), FURA_LFSR_CLK_MUX_3,
                                                  ch4.ERUT_FREQ_10.qp_new(), ETAR_LFSR_CLK_MUX_2,
                                                  ch4.DETE_FREQ_09.qp_new(), EVER_LFSR_CLK_MUX_1,
                                                  ch4.DOSE_FREQ_08.qp_new(), ETOV_LFSR_CLK_MUX_0);

    /*_p20.ELYX*/ wire ELYX_LFSR_CLK_MUX_B = amux4(ch4.DEMO_FREQ_07.qp_new(), EMOF_LFSR_CLK_MUX_7,
                                                  ch4.DOKE_FREQ_06.qp_new(), ELAR_LFSR_CLK_MUX_6,
                                                  ch4.DALE_FREQ_05.qp_new(), DUDU_LFSR_CLK_MUX_5,
                                                  ch4.DURE_FREQ_04.qp_new(), ETAT_LFSR_CLK_MUX_4);

    /*_p20.DARY*/ wire DARY_LFSR_CLK_MUX_C = amux4(ch4.EPOR_FREQ_03.qp_new(), FURA_LFSR_CLK_MUX_3,
                                                  ch4.EZEF_FREQ_02.qp_new(), ETAR_LFSR_CLK_MUX_2,
                                                  ch4.DEKO_FREQ_01.qp_new(), EVER_LFSR_CLK_MUX_1,
                                                  ch4.CEXO_FREQ_00.qp_new(), ETOV_LFSR_CLK_MUX_0);

    /*_p20.ERYF*/ wire ERYF_LFSR_CLK_MUX_D = or2(ELYX_LFSR_CLK_MUX_B, DARY_LFSR_CLK_MUX_C);


    FEME_LFSR_CLKp = mux2p(ch4.GAFO_NR43_FREQ3.qn_new(), ETYR_LFSR_CLK_MUX_A, ERYF_LFSR_CLK_MUX_D);
  }

  {
    /*#p20.JYJA*/ wire JYJA_LFSR_CLKn = not1(FEME_LFSR_CLKp);
    /*#p20.GUFA*/ wire GUFA_LFSR_CLKn = not1(FEME_LFSR_CLKp);
    /*#p20.GYVE*/ wire GYVE_LFSR_CLKp = not1(GUFA_LFSR_CLKn);
    /*#p20.KARA*/ wire KARA_LFSR_CLKn = not1(GYVE_LFSR_CLKp);
    /*#p20.KOPA*/ wire KOPA_LFSR_CLKp = not1(KARA_LFSR_CLKn);

    /*#p20.GEPO*/ wire GEPO_LFSR_RSTa = or2(ch4.GONE_CH4_TRIGp.qp_new(), KEBA_APU_RSTp());
    /*#p20.GOGE*/ wire GOGE_LFSR_RSTn = not1(GEPO_LFSR_RSTa);

    /*#p20.HURA*/ wire HURA_LFSR_IN = xor2(ch4.HEZU_LFSR_15.qp_old(), ch4.HYRO_LFSR_14.qp_old());
    /*#p20.JOTO*/ ch4.JOTO_LFSR_00.dff17(JYJA_LFSR_CLKn, GOGE_LFSR_RSTn, HURA_LFSR_IN);
    /*#p20.KOMU*/ ch4.KOMU_LFSR_01.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.JOTO_LFSR_00.qp_new());
    /*#p20.KETU*/ ch4.KETU_LFSR_02.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.KOMU_LFSR_01.qp_new());
    /*#p20.KUTA*/ ch4.KUTA_LFSR_03.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.KETU_LFSR_02.qp_new());
    /*#p20.KUZY*/ ch4.KUZY_LFSR_04.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.KUTA_LFSR_03.qp_new());
    /*#p20.KYWY*/ ch4.KYWY_LFSR_05.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.KUZY_LFSR_04.qp_new());
    /*#p20.JAJU*/ ch4.JAJU_LFSR_06.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.KYWY_LFSR_05.qp_new());
    /*#p20.HAPE*/ ch4.HAPE_LFSR_07.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.JAJU_LFSR_06.qp_new());
    /*#p20.JUXE*/ ch4.JUXE_LFSR_08.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.HAPE_LFSR_07.qp_new());

    /*#p20.KAVU*/ wire KAVU_LFSR_FB = amux2(ch4.JOTO_LFSR_00.qp_old(), ch4.JAMY_NR43_MODE.qn_old(), ch4.JAMY_NR43_MODE.qp_old(), ch4.JUXE_LFSR_08.qp_old());
    /*#p20.JEPE*/ ch4.JEPE_LFSR_09.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, KAVU_LFSR_FB);
    /*#p20.JAVO*/ ch4.JAVO_LFSR_10.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.JEPE_LFSR_09.qp_new());
    /*#p20.HEPA*/ ch4.HEPA_LFSR_11.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.JAVO_LFSR_10.qp_new());
    /*#p20.HORY*/ ch4.HORY_LFSR_12.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.HEPA_LFSR_11.qp_new());
    /*#p20.HENO*/ ch4.HENO_LFSR_13.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.HORY_LFSR_12.qp_new());
    /*#p20.HYRO*/ ch4.HYRO_LFSR_14.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.HENO_LFSR_13.qp_new());
    /*_p20.HEZU*/ ch4.HEZU_LFSR_15.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, ch4.HYRO_LFSR_14.qp_new());
  }

  //----------
  // Env

  /*#p20.FOWA*/ wire FOWA_ENV_OFFp = nor3(ch4.EMOK_NR42_ENV_TIMER0.qn_new(), ch4.ETYJ_NR42_ENV_TIMER1.qn_new(), ch4.EZYK_NR42_ENV_TIMER2.qn_new());

  {
    /*#p20.ALOP*/ wire ALOP_CLK_128 = not1(BYFE_CLK_128n());
    /*#p20.BOKY*/ wire BOKY_RSTn = not1(KEBA_APU_RSTp());
    /*#p20.ABEL*/ ch4.ABEL_CLK_64.dff17(ALOP_CLK_128, BOKY_RSTn, ch4.ABEL_CLK_64.qn_old());
    /*#p20.BAWA*/ wire BAWA_CLK_64  = not1(ch4.ABEL_CLK_64.qp_new());
    /*#p20.BUXO*/ wire BUXO_CLK_64  = not1(BAWA_CLK_64);

    /*#p20.ENEC*/ wire ENEC_ENV_TIMER_LOADn = nor2(ch4.GONE_CH4_TRIGp.qp_new(), ch4.FOSY_ENV_CLKp.qp_new());
    /*#p20.DAPY*/ wire DAPY_ENV_TIMER_LOADp  = not1(ENEC_ENV_TIMER_LOADn);

    /*#p20.CUNA*/ ch4.CUNA_ENV_DELAY0n.dff20(BUXO_CLK_64,                   DAPY_ENV_TIMER_LOADp, ch4.EMOK_NR42_ENV_TIMER0.qp_old());
    /*#p20.COFE*/ ch4.COFE_ENV_DELAY1n.dff20(ch4.CUNA_ENV_DELAY0n.qp_new(), DAPY_ENV_TIMER_LOADp, ch4.ETYJ_NR42_ENV_TIMER1.qp_old());
    /*#p20.DOGO*/ ch4.DOGO_ENV_DELAY2n.dff20(ch4.COFE_ENV_DELAY1n.qp_new(), DAPY_ENV_TIMER_LOADp, ch4.EZYK_NR42_ENV_TIMER2.qp_old());
  }

  {
    // Generates a 1 usec pulse when the env timer hits 111
    /*#p20.GEXE*/ wire GEXE_ENV_PULSEn  = not1(ch4.FOSY_ENV_CLKp.qp_new());
    /*#p20.HURY*/ wire HURY_ENV_PULSE_RSTp = nor2(HORU_CLK_512p(), GEXE_ENV_PULSEn);
    /*#p20.GOPA*/ wire GOPA_ENV_PULSE_RSTn = nor4(HURY_ENV_PULSE_RSTp, FOWA_ENV_OFFp, ch4.GONE_CH4_TRIGp.qp_new(), KEBA_APU_RSTp());
    /*#p20.EJEX*/ wire EJEX_ENV_TIMER_MAX = or3(ch4.DOGO_ENV_DELAY2n.qp_new(), ch4.COFE_ENV_DELAY1n.qp_new(), ch4.CUNA_ENV_DELAY0n.qp_new());
    /*#p20.FOSY*/ ch4.FOSY_ENV_CLKp.dff17(HORU_CLK_512p(), GOPA_ENV_PULSE_RSTn, EJEX_ENV_TIMER_MAX);
  }

  {
    /*#p20.EMET*/ wire EMET_ENV_STOP_RSTn = nor2(ch4.GONE_CH4_TRIGp.qp_new(), KEBA_APU_RSTp());
    /*#p20.DARO*/ wire DARO_ENV_BOT  = nor5 (ch4.GEKY_NR42_ENV_DIR.qn_new(), ch4.FEKO_CH4_VOL0.qp_new(), ch4.FATY_CH4_VOL1.qp_new(), ch4.FERU_CH4_VOL2.qp_new(), ch4.FYRO_CH4_VOL3.qp_new());
    /*#p20.CUTY*/ wire CUTY_ENV_TOPn = nand5(ch4.GEKY_NR42_ENV_DIR.qn_new(), ch4.FEKO_CH4_VOL0.qp_new(), ch4.FATY_CH4_VOL1.qp_new(), ch4.FERU_CH4_VOL2.qp_new(), ch4.FYRO_CH4_VOL3.qp_new());
    /*#p20.DUBO*/ wire DUBO_ENV_TOPa = not1(CUTY_ENV_TOPn);
    /*#p20.EVUR*/ wire EVUR_ENV_MAXp  = or2(DARO_ENV_BOT, DUBO_ENV_TOPa);
    /*#p20.FYNO*/ ch4.FYNO_ENV_MAXp.dff17(ch4.FOSY_ENV_CLKp.qp_new(), EMET_ENV_STOP_RSTn, EVUR_ENV_MAXp);
  }





  {
    /*#p20.ENUR*/ wire ENUR_ENV_STARTp = or2(KEBA_APU_RSTp(), ch4.GONE_CH4_TRIGp.qp_new());
    /*#p20.EROX*/ ch4.EROX_ENV_RUNNINGn.nor_latch(ch4.FYNO_ENV_MAXp.qp_new(), ENUR_ENV_STARTp);

    /*#p20.FELO*/ wire FELO_ENV_CLK  = or3(ch4.FOSY_ENV_CLKp.qp_new(), FOWA_ENV_OFFp, ch4.EROX_ENV_RUNNINGn.qp_new());
    /*#p20.FOLE*/ wire FOLE_VOL_CLK0n = amux2(ch4.GEKY_NR42_ENV_DIR.qn_new(), FELO_ENV_CLK,               FELO_ENV_CLK,               ch4.GEKY_NR42_ENV_DIR.qp_new());
    /*#p20.ETEF*/ wire ETEF_VOL_CLK1n = amux2(ch4.GEKY_NR42_ENV_DIR.qn_new(), ch4.FEKO_CH4_VOL0.qp_new(), ch4.FEKO_CH4_VOL0.qn_new(), ch4.GEKY_NR42_ENV_DIR.qp_new());
    /*#p20.EDYF*/ wire EDYF_VOL_CLK2n = amux2(ch4.GEKY_NR42_ENV_DIR.qn_new(), ch4.FATY_CH4_VOL1.qp_new(), ch4.FATY_CH4_VOL1.qn_new(), ch4.GEKY_NR42_ENV_DIR.qp_new());
    /*#p20.ELAF*/ wire ELAF_VOL_CLK3n = amux2(ch4.GEKY_NR42_ENV_DIR.qn_new(), ch4.FERU_CH4_VOL2.qp_new(), ch4.FERU_CH4_VOL2.qn_new(), ch4.GEKY_NR42_ENV_DIR.qp_new());

    /*#p20.FEKO*/ ch4.FEKO_CH4_VOL0.dff20(FOLE_VOL_CLK0n, ch4.GONE_CH4_TRIGp.qp_new(), ch4.GARU_NR42_ENV0.qn_old());
    /*#p20.FATY*/ ch4.FATY_CH4_VOL1.dff20(ETEF_VOL_CLK1n, ch4.GONE_CH4_TRIGp.qp_new(), ch4.GOKY_NR42_ENV1.qn_old());
    /*#p20.FERU*/ ch4.FERU_CH4_VOL2.dff20(EDYF_VOL_CLK2n, ch4.GONE_CH4_TRIGp.qp_new(), ch4.GOZO_NR42_ENV2.qn_old());
    /*#p20.FYRO*/ ch4.FYRO_CH4_VOL3.dff20(ELAF_VOL_CLK3n, ch4.GONE_CH4_TRIGp.qp_new(), ch4.GEDU_NR42_ENV3.qn_old());
  }

  {
    //----------
    // Output

    /*#p20.GAME*/ wire GAME_LFSR_OUT = and2(ch4.GENA_CH4_ACTIVEp.qp_new(), ch4.HEZU_LFSR_15.qp_new());
    /*#p20.EZUL*/ wire EZUL_CH4_BIT_MUX = mux2p(DYRY_DBG, FEME_LFSR_CLKp, GAME_LFSR_OUT);
    /*#p20.CEPY*/ wire CEPY_NR44_STOPn = not1(ch4.CUNY_NR44_LEN_ENp.qn_new());
    /*#p20.COTE*/ wire COTE_DBG_CH4_MUTE = and2(CEPY_NR44_STOPn, EDEK_NR52_DBG_APUn);
    /*#p20.DATO*/ wire DATO_CH4_RAW_BIT  = or2(EZUL_CH4_BIT_MUX, COTE_DBG_CH4_MUTE);

    /*#p20.AKOF*/ wire AKOF_CH4_DAC0 = and2(ch4.FEKO_CH4_VOL0.qp_new(), DATO_CH4_RAW_BIT);
    /*#p20.BYZY*/ wire BYZY_CH4_DAC1 = and2(ch4.FATY_CH4_VOL1.qp_new(), DATO_CH4_RAW_BIT);
    /*#p20.APYR*/ wire APYR_CH4_DAC2 = and2(ch4.FERU_CH4_VOL2.qp_new(), DATO_CH4_RAW_BIT);
    /*#p20.BOZA*/ wire BOZA_CH4_DAC3 = and2(ch4.FYRO_CH4_VOL3.qp_new(), DATO_CH4_RAW_BIT);
  }

















































































  ///////////////////
  // CHANNEL 1
  ///////////////////


  //----------
  // FF10 NR10

  {
    /*#p10.DUPO*/ wire DUPO_ADDR_0000n = nand4(ACOL_A03n(), ABUB_A02n(), AFOB_A01n(), DYTE_A00n());
    /*#p10.DYVA*/ wire DYVA_ADDR_FF10p = nor2(BANU_ADDR_FF1Xn(), DUPO_ADDR_0000n);
    /*#p11.BUZE*/ wire BUZE_ADDR_FF10n = not1(DYVA_ADDR_FF10p);

    /*#p11.CENU*/ wire CENU_NR10_WRp = and2(BOGY_CPU_WRp(), DYVA_ADDR_FF10p);
    /*#p11.CENU*/ wire CENU_NR10_WRn = not1(CENU_NR10_WRp);

    /*#p11.BANY*/ ch1.BANY_NR10_SWEEP_SHIFT0  .dff9(CENU_NR10_WRn, AGUR_APU_RSTn(), BUS_CPU_D00p.qp_new());
    /*#p11.ARAX*/ ch1.ARAX_NR10_SWEEP_SHIFT1  .dff9(CENU_NR10_WRn, AGUR_APU_RSTn(), BUS_CPU_D01p.qp_new());
    /*#p11.ANAZ*/ ch1.ANAZ_NR10_SWEEP_SHIFT2  .dff9(CENU_NR10_WRn, AGUR_APU_RSTn(), BUS_CPU_D02p.qp_new());
    /*#p11.AVAF*/ ch1.AVAF_NR10_SWEEP_NEGATE  .dff9(CENU_NR10_WRn, AGUR_APU_RSTn(), BUS_CPU_D03p.qp_new());
    /*#p11.ADEK*/ ch1.ADEK_NR10_SWEEP_PERIOD0p.dff9(CENU_NR10_WRn, AGUR_APU_RSTn(), BUS_CPU_D04p.qp_new());
    /*#p11.BANA*/ ch1.BANA_NR10_SWEEP_PERIOD1p.dff9(CENU_NR10_WRn, AGUR_APU_RSTn(), BUS_CPU_D05p.qp_new());
    /*#p11.BOTU*/ ch1.BOTU_NR10_SWEEP_PERIOD2p.dff9(CENU_NR10_WRn, AGUR_APU_RSTn(), BUS_CPU_D06p.qp_new());

    /*#p11.ATYN*/ wire ATYN_NR10_RDp = nor2(AGUZ_CPU_RDn(), BUZE_ADDR_FF10n);
    /*#p11.ASOP*/ wire ASOP_NR10_RDn = not1(ATYN_NR10_RDp);

    // Isn't the data polarity here backwards?
    /*#p11.AMYD*/ triwire AMYD_NR10_SWEEP_SHIFT0  = tri6_nn(ASOP_NR10_RDn, ch1.BANY_NR10_SWEEP_SHIFT0.qp_new());
    /*#p11.ATAX*/ triwire ATAX_NR10_SWEEP_SHIFT1  = tri6_nn(ASOP_NR10_RDn, ch1.ARAX_NR10_SWEEP_SHIFT1.qp_new());
    /*#p11.AZYP*/ triwire AZYP_NR10_SWEEP_SHIFT2  = tri6_nn(ASOP_NR10_RDn, ch1.ANAZ_NR10_SWEEP_SHIFT2.qp_new());
    /*#p11.AFOX*/ triwire AFOX_NR10_SWEEP_NEGATE  = tri6_nn(ASOP_NR10_RDn, ch1.AVAF_NR10_SWEEP_NEGATE.qp_new());
    /*#p11.AVEK*/ triwire AVEK_NR10_SWEEP_PERIOD0 = tri6_nn(ASOP_NR10_RDn, ch1.ADEK_NR10_SWEEP_PERIOD0p.qp_new());
    /*#p11.AKUX*/ triwire AKUX_NR10_SWEEP_PERIOD1 = tri6_nn(ASOP_NR10_RDn, ch1.BANA_NR10_SWEEP_PERIOD1p.qp_new());
    /*#p11.AWOS*/ triwire AWOS_NR10_SWEEP_PERIOD2 = tri6_nn(ASOP_NR10_RDn, ch1.BOTU_NR10_SWEEP_PERIOD2p.qp_new());
  }

  /*#p12.ARYL*/ wire ARYL_NR10_SWEEP_DIRn = not1(ch1.AVAF_NR10_SWEEP_NEGATE.qp_new());

  tick_nr11();

  //----------
  // FF12 NR12

  {
  }

  /*#p13.HOCA*/ wire HOCA_CH1_AMP_ENn = nor5(ch1.JAFY_NR12_ENV_DIR.qn_new(),
                                             ch1.JATY_NR12_VOL0.qn_new(),
                                             ch1.JAXO_NR12_VOL1.qn_new(),
                                             ch1.JENA_NR12_VOL2.qn_new(),
                                             ch1.JOPU_NR12_VOL3.qn_new());

  //----------
  // Frequency can be read from FF13/FF14 in debug mode

  {
    /*#p11.DYPU*/ wire DYPU_FF13_RDn = nor2(CACA_ADDR_FF13n(), DAXA_CPU_RDn_DBG);
    /*#p11.EVAJ*/ wire EVAJ_FF13_RDa = not1(DYPU_FF13_RDn);

    /*_p11.FORU*/ triwire FORU = tri6_nn(EVAJ_FF13_RDa, ch1.GAXE_CH1_FREQ_CNT_00.qn_new());
    /*_p11.GEFU*/ triwire GEFU = tri6_nn(EVAJ_FF13_RDa, ch1.HYFE_CH1_FREQ_CNT_01.qn_new());
    /*_p11.KYVU*/ triwire KYVU = tri6_nn(EVAJ_FF13_RDa, ch1.JYTY_CH1_FREQ_CNT_02.qn_new());
    /*_p11.KUMO*/ triwire KUMO = tri6_nn(EVAJ_FF13_RDa, ch1.KYNA_CH1_FREQ_CNT_03.qn_new());
    /*_p11.KARY*/ triwire KARY = tri6_nn(EVAJ_FF13_RDa, ch1.JEMA_CH1_FREQ_CNT_04.qn_new());
    /*_p11.GODE*/ triwire GODE = tri6_nn(EVAJ_FF13_RDa, ch1.HYKE_CH1_FREQ_CNT_05.qn_new());
    /*_p11.GOJE*/ triwire GOJE = tri6_nn(EVAJ_FF13_RDa, ch1.FEVA_CH1_FREQ_CNT_06.qn_new());
    /*_p11.FOZE*/ triwire FOZE = tri6_nn(EVAJ_FF13_RDa, ch1.EKOV_CH1_FREQ_CNT_07.qn_new());
  }

  //----------
  // FF14
  // Frequency can be read from FF13/FF14 in debug mode

  {
    /*#p13.DADO*/ wire DADO_NR14_RSTb = nor2(KEBA_APU_RSTp(), ch1.EZEC_CH1_TRIGn.qp_new());

    /*#p11.BAGE*/ wire BAGE_NR14_WRn = nand2(ANUJ_CPU_WR_WEIRD, DUJA_ADDR_FF14p());
    /*#p13.DOGE*/ wire DOGE_FF14_WRn = nand2(BOGY_CPU_WRp(), DUJA_ADDR_FF14p());

    /*#p11.BOKO*/ ch1.BOKO_NR14_LEN_EN.dff9(BAGE_NR14_WRn, CAMY_APU_RSTn(), BUS_CPU_D06p.qp_new());
    /*#p13.DUPE*/ ch1.DUPE_NR14_TRIG  .dff9(DOGE_FF14_WRn, DADO_NR14_RSTb, BUS_CPU_D07p.qp_new());
  }

  {
    /*#p11.BUDA*/ wire BUDA_CPU_RDp = not1(AGUZ_CPU_RDn());
    /*#p11.BALE*/ wire BALE_NR14_RDn = nand2(DUJA_ADDR_FF14p(), BUDA_CPU_RDp);
    /*#p11.DUPY*/ wire DUPY_FF14_RDn = or2(CURE_ADDR_FF14n(), DAXA_CPU_RDn_DBG);
 
    /*#p11.BYTU*/ triwire BYTU = tri6_nn(BALE_NR14_RDn, ch1.BOKO_NR14_LEN_EN.qp_new());
    /*_p11.DOPA*/ triwire DOPA = tri6_nn(DUPY_FF14_RDn, ch1.EMUS_CH1_FREQ_CNT_08.qn_new());
    /*_p11.DEMU*/ triwire DEMU = tri6_nn(DUPY_FF14_RDn, ch1.EVAK_CH1_FREQ_CNT_09.qn_new());
    /*_p11.DEXO*/ triwire DEXO = tri6_nn(DUPY_FF14_RDn, ch1.COPU_CH1_FREQ_CNT_10.qn_new());
  }

  //----------
  // Sweep timer

  {
    /*#p09.BAZA*/ ch1.BAZA_DBG_SWEEP_CLK.dff17(spu.AJER_2M.qn_new(), ATYV_APU_RSTn(), spu.BOWY_NR52_DBG_SWEEP.qp_old());
  }

  {
    /*#p09.CELY*/ wire CELY_SWEEP_DELAY_CLKn = mux2p(EDEK_NR52_DBG_APUn, ch1.BAZA_DBG_SWEEP_CLK.qp_new(), BYFE_CLK_128n());
    /*#p09.CONE*/ wire CONE_SWEEP_DELAY_CLKp = not1(CELY_SWEEP_DELAY_CLKn);
    /*#p09.CATE*/ wire CATE_SWEEP_DELAY_CLKn = not1(CONE_SWEEP_DELAY_CLKp);

    /*#p13.DAFA*/ wire DAFA_SWEEP_DELAY_LOADn = nor2(ch1.BEXA_SWEEP_TRIGGERp.qp_new(), ch1.FEKU_CH1_TRIGp.qp_new());
    /*#p13.CYMU*/ wire CYMU_SWEEP_DELAY_LOADp = not1(DAFA_SWEEP_DELAY_LOADn);

    /*#p13.CUPO*/ ch1.CUPO_SWEEP_DELAY0p.dff20(CATE_SWEEP_DELAY_CLKn,           CYMU_SWEEP_DELAY_LOADp, ch1.ADEK_NR10_SWEEP_PERIOD0p.qp_old());
    /*#p13.CYPU*/ ch1.CYPU_SWEEP_DELAY1p.dff20(ch1.CUPO_SWEEP_DELAY0p.qp_new(), CYMU_SWEEP_DELAY_LOADp, ch1.BANA_NR10_SWEEP_PERIOD1p.qp_old());
    /*#p13.CAXY*/ ch1.CAXY_SWEEP_DELAY2p.dff20(ch1.CYPU_SWEEP_DELAY1p.qp_new(), CYMU_SWEEP_DELAY_LOADp, ch1.BOTU_NR10_SWEEP_PERIOD2p.qp_old());
  }

  {
    /*#p13.BAVE*/ wire BAVE_NR10_NO_SWEEPp = and3(ch1.BOTU_NR10_SWEEP_PERIOD2p.qp_new(), ch1.BANA_NR10_SWEEP_PERIOD1p.qp_new(), ch1.ADEK_NR10_SWEEP_PERIOD0p.qp_new());
    /*#p13.BURY*/ wire BURY_SWEEP_TRIGGER_RSTn = nor2(BAVE_NR10_NO_SWEEPp, KEBA_APU_RSTp());
    /*#p13.COZE*/ wire COZE_SWEEP_MAX = and3(ch1.CAXY_SWEEP_DELAY2p.qp_new(), ch1.CYPU_SWEEP_DELAY1p.qp_new(), ch1.CUPO_SWEEP_DELAY0p.qp_new());

    /*#p13.BEXA*/ ch1.BEXA_SWEEP_TRIGGERp.dff17(spu.AJER_2M.qp_new(), BURY_SWEEP_TRIGGER_RSTn, COZE_SWEEP_MAX);
  }

  //----------
  // Frequency summer

  {
    /*#p13.ATAT*/ wire ATAT_SWEEP_RSTn = nor2(KEBA_APU_RSTp(), ch1.BEXA_SWEEP_TRIGGERp.qp_new());
    /*#p13.COPY*/ wire COPY_SHIFT_DONE_old = and3(ch1.BYRA_SHIFTCNT2.qp_old(), ch1.CAJA_SHIFTCNT1.qp_old(), ch1.COPA_SHIFTCNT0.qp_old());
    /*#p13.BYTE*/ ch1.BYTE_SHIFT_DONE.dff17(spu.AJER_2M.qp_new(), ATAT_SWEEP_RSTn, COPY_SHIFT_DONE_old);
  }
  /*#p13.ADAD*/ wire ADAD_SHIFT_DONE = not1(ch1.BYTE_SHIFT_DONE.qn_new());


  {
    /*#p13.KABA*/ wire KABA_SUM_RST = or2(KEBA_APU_RSTp(), ch1.FEKU_CH1_TRIGp.qp_new());
    /*#p13.KYNY*/ wire KYNY_SUM_RSTn = not1(KABA_SUM_RST);

    /*_p12.GALO*/ ch1.GALO_SUM_A00.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.HYKA_CH1_FREQ00.qp_old());
    /*_p12.JODE*/ ch1.JODE_SUM_A01.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.JYKA_CH1_FREQ01.qp_old());
    /*_p12.KARE*/ ch1.KARE_SUM_A02.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.HAVO_CH1_FREQ02.qp_old());
    /*_p12.JYME*/ ch1.JYME_SUM_A03.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.EDUL_CH1_FREQ03.qp_old());
    /*_p12.GYME*/ ch1.GYME_SUM_A04.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.FELY_CH1_FREQ04.qp_old());
    /*_p12.FAXO*/ ch1.FAXO_SUM_A05.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.HOLU_CH1_FREQ05.qp_old());
    /*_p12.EXAP*/ ch1.EXAP_SUM_A06.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.HYXU_CH1_FREQ06.qp_old());
    /*_p12.DELE*/ ch1.DELE_SUM_A07.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.HOPO_CH1_FREQ07.qp_old());
    /*_p12.DEXE*/ ch1.DEXE_SUM_A08.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.DYGY_CH1_FREQ08.qp_old());
    /*_p12.DOFY*/ ch1.DOFY_SUM_A09.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.EVAB_CH1_FREQ09.qp_old());
    /*_p12.DOLY*/ ch1.DOLY_SUM_A10.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, ch1.AXAN_CH1_FREQ10.qp_old());

    /*_p12.GYLO*/ wire GYLO_SHIFTER_FLIP_00 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.FABU_CH1_SHIFT00.qp_new());
    /*_p12.GELA*/ wire GELA_SHIFTER_FLIP_01 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.JEFA_CH1_SHIFT01.qp_new());
    /*_p12.GOPO*/ wire GOPO_SHIFTER_FLIP_02 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.GOGA_CH1_SHIFT02.qp_new());
    /*_p12.HEFY*/ wire HEFY_SHIFTER_FLIP_03 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.JOLU_CH1_SHIFT03.qp_new());
    /*_p12.KEFE*/ wire KEFE_SHIFTER_FLIP_04 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.JOTA_CH1_SHIFT04.qp_new());
    /*_p12.GOLY*/ wire GOLY_SHIFTER_FLIP_05 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.FUDE_CH1_SHIFT05.qp_new());
    /*_p12.FURE*/ wire FURE_SHIFTER_FLIP_06 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.FEDO_CH1_SHIFT06.qp_new());
    /*_p12.DYME*/ wire DYME_SHIFTER_FLIP_07 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.EXAC_CH1_SHIFT07.qp_new());
    /*_p12.CALE*/ wire CALE_SHIFTER_FLIP_08 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.ELUX_CH1_SHIFT08.qp_new());
    /*_p12.DOZY*/ wire DOZY_SHIFTER_FLIP_09 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.AGEZ_CH1_SHIFT09.qp_new());
    /*_p12.CULU*/ wire CULU_SHIFTER_FLIP_10 = xor2(ARYL_NR10_SWEEP_DIRn, ch1.BEKU_CH1_SHIFT10.qp_new());

    /*_p12.HORA*/ ch1.HORA_SUM_B00.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, GYLO_SHIFTER_FLIP_00);
    /*_p12.HOPA*/ ch1.HOPA_SUM_B01.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, GELA_SHIFTER_FLIP_01);
    /*_p12.HELE*/ ch1.HELE_SUM_B02.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, GOPO_SHIFTER_FLIP_02);
    /*_p12.JAPE*/ ch1.JAPE_SUM_B03.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, HEFY_SHIFTER_FLIP_03);
    /*_p12.JETE*/ ch1.JETE_SUM_B04.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, KEFE_SHIFTER_FLIP_04);
    /*_p12.GELE*/ ch1.GELE_SUM_B05.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, GOLY_SHIFTER_FLIP_05);
    /*_p12.EPYR*/ ch1.EPYR_SUM_B06.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, FURE_SHIFTER_FLIP_06);
    /*_p12.EDOK*/ ch1.EDOK_SUM_B07.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, DYME_SHIFTER_FLIP_07);
    /*_p12.DEFA*/ ch1.DEFA_SUM_B08.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, CALE_SHIFTER_FLIP_08);
    /*_p12.ETER*/ ch1.ETER_SUM_B09.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, DOZY_SHIFTER_FLIP_09);
    /*_p12.DEVA*/ ch1.DEVA_SUM_B10.dff11(ADAD_SHIFT_DONE, KYNY_SUM_RSTn, CULU_SHIFTER_FLIP_10);
  
  }

  /*#p12.GUXA*/ Adder GUXA_SUM00 = add3(ch1.GALO_SUM_A00.qp_new(), ch1.HORA_SUM_B00.qp_new(), ARYL_NR10_SWEEP_DIRn);
  /*_p12.HALU*/ Adder HALU_SUM01 = add3(ch1.JODE_SUM_A01.qp_new(), ch1.HOPA_SUM_B01.qp_new(), GUXA_SUM00.carry);
  /*_p12.JULE*/ Adder JULE_SUM02 = add3(ch1.KARE_SUM_A02.qp_new(), ch1.HELE_SUM_B02.qp_new(), HALU_SUM01.carry);
  /*_p12.JORY*/ Adder JORY_SUM03 = add3(ch1.JYME_SUM_A03.qp_new(), ch1.JAPE_SUM_B03.qp_new(), JULE_SUM02.carry);
  /*_p12.HEXO*/ Adder HEXO_SUM04 = add3(ch1.GYME_SUM_A04.qp_new(), ch1.JETE_SUM_B04.qp_new(), JORY_SUM03.carry);
  /*_p12.GEVA*/ Adder GEVA_SUM05 = add3(ch1.FAXO_SUM_A05.qp_new(), ch1.GELE_SUM_B05.qp_new(), HEXO_SUM04.carry);
  /*#p12.FEGO*/ Adder FEGO_SUM06 = add3(ch1.EXAP_SUM_A06.qp_new(), ch1.EPYR_SUM_B06.qp_new(), GEVA_SUM05.carry);
  /*_p12.ETEK*/ Adder ETEK_SUM07 = add3(ch1.DELE_SUM_A07.qp_new(), ch1.EDOK_SUM_B07.qp_new(), FEGO_SUM06.carry);
  /*_p12.DYXE*/ Adder DYXE_SUM08 = add3(ch1.DEXE_SUM_A08.qp_new(), ch1.DEFA_SUM_B08.qp_new(), ETEK_SUM07.carry);
  /*_p12.DULE*/ Adder DULE_SUM09 = add3(ch1.DOFY_SUM_A09.qp_new(), ch1.ETER_SUM_B09.qp_new(), DYXE_SUM08.carry);
  /*_p12.CORU*/ Adder CORU_SUM10 = add3(ch1.DOLY_SUM_A10.qp_new(), ch1.DEVA_SUM_B10.qp_new(), DULE_SUM09.carry);

  /*#p12.BYLE*/ wire BYLE_FREQ_OVERFLOW = nor2(ARYL_NR10_SWEEP_DIRn, CORU_SUM10.carry);
  /*#p12.ATYS*/ wire ATYS_FREQ_OVERFLOW = or2(BYLE_FREQ_OVERFLOW, ARYL_NR10_SWEEP_DIRn);


  /*#p13.BUGE*/ wire BUGE_SWEEP_ENn = nand3(ch1.ANAZ_NR10_SWEEP_SHIFT2.qp_new(),
                                            ch1.ARAX_NR10_SWEEP_SHIFT1.qp_new(),
                                            ch1.BANY_NR10_SWEEP_SHIFT0.qp_new());

  {

    /*#p12.DEBY*/ wire DEBY_FF14_WRp = and2(BOGY_CPU_WRp(), DUJA_ADDR_FF14p());
    /*#p12.DEPU*/ wire DEPU_FF13_WRn = nand2(BOGY_CPU_WRp(), DECO_ADDR_FF13p());
    /*#p12.DYLA*/ wire DYLA_FF13_WRp = not1(DEPU_FF13_WRn);

    /*#p12.GYLU*/ wire GYLU_FREQ00_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D00p.qp_new());
    /*#p12.GETA*/ wire GETA_FREQ01_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D01p.qp_new());
    /*#p12.GOLO*/ wire GOLO_FREQ02_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D02p.qp_new());
    /*#p12.GOPE*/ wire GOPE_FREQ03_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D03p.qp_new());
    /*#p12.KOVU*/ wire KOVU_FREQ04_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D04p.qp_new());
    /*#p12.KYPA*/ wire KYPA_FREQ05_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D05p.qp_new());
    /*#p12.ELER*/ wire ELER_FREQ06_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D06p.qp_new());
    /*#p12.ETOL*/ wire ETOL_FREQ07_SET = nand2(DYLA_FF13_WRp, BUS_CPU_D07p.qp_new());
    /*#p12.BUGU*/ wire BUGU_FREQ08_SET = nand2(DEBY_FF14_WRp, BUS_CPU_D00p.qp_new());
    /*#p12.BUDO*/ wire BUDO_FREQ09_SET = nand2(DEBY_FF14_WRp, BUS_CPU_D01p.qp_new());
    /*#p12.AFEG*/ wire AFEG_FREQ10_SET = nand2(DEBY_FF14_WRp, BUS_CPU_D02p.qp_new());

    /*#p12.DEKE*/ wire DEKE = not1(BUS_CPU_D00p.qp_new());
    /*#p12.GULU*/ wire GULU = not1(BUS_CPU_D01p.qp_new());
    /*#p12.FULE*/ wire FULE = not1(BUS_CPU_D02p.qp_new());
    /*#p12.ETUV*/ wire ETUV = not1(BUS_CPU_D03p.qp_new());
    /*#p12.KOPU*/ wire KOPU = not1(BUS_CPU_D04p.qp_new());
    /*#p12.JULO*/ wire JULO = not1(BUS_CPU_D05p.qp_new());
    /*#p12.DYLU*/ wire DYLU = not1(BUS_CPU_D06p.qp_new());
    /*#p12.DULO*/ wire DULO = not1(BUS_CPU_D07p.qp_new());
    /*#p12.BYSU*/ wire BYSU = not1(BUS_CPU_D00p.qp_new());
    /*#p12.BOFU*/ wire BOFU = not1(BUS_CPU_D01p.qp_new());
    /*#p12.BYFU*/ wire BYFU = not1(BUS_CPU_D02p.qp_new());

    /*#p12.EJYF*/ wire EJYF = and2(DYLA_FF13_WRp, DEKE);
    /*#p12.FOPU*/ wire FOPU = and2(DYLA_FF13_WRp, GULU);
    /*#p12.FOKE*/ wire FOKE = and2(DYLA_FF13_WRp, FULE);
    /*#p12.FEGA*/ wire FEGA = and2(DYLA_FF13_WRp, ETUV);
    /*#p12.KAVO*/ wire KAVO = and2(DYLA_FF13_WRp, KOPU);
    /*#p12.KYFU*/ wire KYFU = and2(DYLA_FF13_WRp, JULO);
    /*#p12.ETOK*/ wire ETOK = and2(DYLA_FF13_WRp, DYLU);
    /*#p12.EMAR*/ wire EMAR = and2(DYLA_FF13_WRp, DULO);
    /*#p12.BASO*/ wire BASO = and2(DEBY_FF14_WRp, BYSU);
    /*#p12.AMAC*/ wire AMAC = and2(DEBY_FF14_WRp, BOFU);
    /*#p12.AJUX*/ wire AJUX = and2(DEBY_FF14_WRp, BYFU);

    /*#p12.EFOR*/ wire EFOR_FREQ00_RST = nor2(EJYF, KEBA_APU_RSTp()); 
    /*#p12.GATO*/ wire GATO_FREQ01_RST = nor2(FOPU, KEBA_APU_RSTp()); 
    /*#p12.GYFU*/ wire GYFU_FREQ02_RST = nor2(FOKE, KEBA_APU_RSTp()); 
    /*#p12.GAMO*/ wire GAMO_FREQ03_RST = nor2(FEGA, KEBA_APU_RSTp()); 
    /*#p12.KAPO*/ wire KAPO_FREQ04_RST = nor2(KAVO, KEBA_APU_RSTp()); 
    /*#p12.KAJU*/ wire KAJU_FREQ05_RST = nor2(KYFU, KEBA_APU_RSTp()); 
    /*#p12.ELUF*/ wire ELUF_FREQ06_RST = nor2(ETOK, KEBA_APU_RSTp()); 
    /*#p12.ESEL*/ wire ESEL_FREQ07_RST = nor2(EMAR, KEBA_APU_RSTp()); 
    /*#p12.BOXU*/ wire BOXU_FREQ08_RST = nor2(BASO, KEBA_APU_RSTp()); 
    /*#p12.BOVU*/ wire BOVU_FREQ09_RST = nor2(AMAC, KEBA_APU_RSTp()); 
    /*#p12.APAJ*/ wire APAJ_FREQ10_RST = nor2(AJUX, KEBA_APU_RSTp()); 

    /*#p13.ATUV*/ wire ATUV_FREQ_CLK = and2(ch1.BEXA_SWEEP_TRIGGERp.qp_new(), ATYS_FREQ_OVERFLOW);
    /*#p13.BOJE*/ wire BOJE_FREQ_CLK = and2(ATUV_FREQ_CLK, BUGE_SWEEP_ENn);
    /*#p13.BUSO*/ wire BUSO_FREQ_CLK = and3(ch1.BEXA_SWEEP_TRIGGERp.qp_new(), ATYS_FREQ_OVERFLOW, BUGE_SWEEP_ENn);

    /*_p12.HYKA*/ ch1.HYKA_CH1_FREQ00.dff22(BOJE_FREQ_CLK, GYLU_FREQ00_SET, EFOR_FREQ00_RST, GUXA_SUM00.sum);
    /*_p12.JYKA*/ ch1.JYKA_CH1_FREQ01.dff22(BOJE_FREQ_CLK, GETA_FREQ01_SET, GATO_FREQ01_RST, HALU_SUM01.sum);
    /*_p12.HAVO*/ ch1.HAVO_CH1_FREQ02.dff22(BOJE_FREQ_CLK, GOLO_FREQ02_SET, GYFU_FREQ02_RST, JULE_SUM02.sum);
    /*_p12.EDUL*/ ch1.EDUL_CH1_FREQ03.dff22(BOJE_FREQ_CLK, GOPE_FREQ03_SET, GAMO_FREQ03_RST, JORY_SUM03.sum);
    /*_p12.FELY*/ ch1.FELY_CH1_FREQ04.dff22(BOJE_FREQ_CLK, KOVU_FREQ04_SET, KAPO_FREQ04_RST, HEXO_SUM04.sum);
    /*_p12.HOLU*/ ch1.HOLU_CH1_FREQ05.dff22(BOJE_FREQ_CLK, KYPA_FREQ05_SET, KAJU_FREQ05_RST, GEVA_SUM05.sum);
    /*_p12.HYXU*/ ch1.HYXU_CH1_FREQ06.dff22(BOJE_FREQ_CLK, ELER_FREQ06_SET, ELUF_FREQ06_RST, FEGO_SUM06.sum);
    /*_p12.HOPO*/ ch1.HOPO_CH1_FREQ07.dff22(BOJE_FREQ_CLK, ETOL_FREQ07_SET, ESEL_FREQ07_RST, ETEK_SUM07.sum);
    /*_p12.DYGY*/ ch1.DYGY_CH1_FREQ08.dff22(BUSO_FREQ_CLK, BUGU_FREQ08_SET, BOXU_FREQ08_RST, DYXE_SUM08.sum);
    /*_p12.EVAB*/ ch1.EVAB_CH1_FREQ09.dff22(BUSO_FREQ_CLK, BUDO_FREQ09_SET, BOVU_FREQ09_RST, DULE_SUM09.sum);
    /*_p12.AXAN*/ ch1.AXAN_CH1_FREQ10.dff22(BUSO_FREQ_CLK, AFEG_FREQ10_SET, APAJ_FREQ10_RST, CORU_SUM10.sum);
  }

  //----------
  // Frequency shifter

  {
    /*#p13.EPUK*/ wire EPUK_SETn = nor2(KEBA_APU_RSTp(), ADAD_SHIFT_DONE);
    /*#p13.EVOL*/ wire EVOL_RSTn = nor2(ch1.BEXA_SWEEP_TRIGGERp.qp_new(), ch1.FYTE_CH1_TRIGp.qp_new());
    /*#p13.FEMU*/ ch1.FEMU_SHIFTINGn.nand_latch(EPUK_SETn, EVOL_RSTn);
  }

  /*#p13.EGYP*/   wire EGYP_SHIFT_CLK = nor2(ch1.FEMU_SHIFTINGn.qn_new(), DYFA_CLK_1M);
  /*#p13.CELE*/   wire CELE_SWEEP_ENp = not1(BUGE_SWEEP_ENn);
  /*#p13.DODY*/   wire DODY_SHIFT_CLK = nor2(EGYP_SHIFT_CLK, CELE_SWEEP_ENp); // border color wrong on die
  /*?p13.EGOR*/ wire EGOR_SHIFT_CLK = not1(DODY_SHIFT_CLK); // This looks like a nor3, but it almost definiteily is a not1.

  {
    /*#p13.DAPU*/ wire DAPU_SHIFT_CLK = not1(EGOR_SHIFT_CLK);
    /*#p13.DACU*/ wire DACU_SHIFTCNT_LOADn = nor2(ch1.FEKU_CH1_TRIGp.qp_new(), ch1.BEXA_SWEEP_TRIGGERp.qp_new());
    /*#p13.CYLU*/ wire CYLU_SHIFTCNT_LOADp = not1(DACU_SHIFTCNT_LOADn);
    /*#p13.COPA*/ ch1.COPA_SHIFTCNT0.dff20(DAPU_SHIFT_CLK,              CYLU_SHIFTCNT_LOADp, ch1.BANY_NR10_SWEEP_SHIFT0.qp_new());
    /*#p13.CAJA*/ ch1.CAJA_SHIFTCNT1.dff20(ch1.COPA_SHIFTCNT0.qp_new(), CYLU_SHIFTCNT_LOADp, ch1.ARAX_NR10_SWEEP_SHIFT1.qp_new());
    /*#p13.BYRA*/ ch1.BYRA_SHIFTCNT2.dff20(ch1.CAJA_SHIFTCNT1.qp_new(), CYLU_SHIFTCNT_LOADp, ch1.ANAZ_NR10_SWEEP_SHIFT2.qp_new());
  }

  {
    /*#p13.KALA*/ wire KALA_SHIFTER_LOADn = nor2(ch1.BEXA_SWEEP_TRIGGERp.qp_new(), ch1.FEKU_CH1_TRIGp.qp_new());
    /*#p12.KAPE*/ wire KAPE_SHIFTER_LOADp = not1(KALA_SHIFTER_LOADn);
    /*#p12.JUJU*/ wire JUJU_SHIFTER_LOADp = not1(KALA_SHIFTER_LOADn);
    /*#p12.KEDO*/ wire KEDO_SHIFTER_LOADp = not1(KALA_SHIFTER_LOADn);

    /*_p12.HOZU*/ wire HOZU_SHIFTER_SETn_00 = nand2(ch1.HYKA_CH1_FREQ00.qp_new(), KAPE_SHIFTER_LOADp);
    /*_p12.HOLA*/ wire HOLA_SHIFTER_SETn_01 = nand2(ch1.JYKA_CH1_FREQ01.qp_new(), KAPE_SHIFTER_LOADp);
    /*_p12.HAWY*/ wire HAWY_SHIFTER_SETn_02 = nand2(ch1.HAVO_CH1_FREQ02.qp_new(), KAPE_SHIFTER_LOADp);
    /*_p12.KYRY*/ wire KYRY_SHIFTER_SETn_03 = nand2(ch1.EDUL_CH1_FREQ03.qp_new(), JUJU_SHIFTER_LOADp);
    /*_p12.KOLA*/ wire KOLA_SHIFTER_SETn_04 = nand2(ch1.FELY_CH1_FREQ04.qp_new(), JUJU_SHIFTER_LOADp);
    /*_p12.GOVO*/ wire GOVO_SHIFTER_SETn_05 = nand2(ch1.HOLU_CH1_FREQ05.qp_new(), JUJU_SHIFTER_LOADp);
    /*_p12.EKEM*/ wire EKEM_SHIFTER_SETn_06 = nand2(ch1.HYXU_CH1_FREQ06.qp_new(), JUJU_SHIFTER_LOADp);
    /*_p12.DACE*/ wire DACE_SHIFTER_SETn_07 = nand2(ch1.HOPO_CH1_FREQ07.qp_new(), KEDO_SHIFTER_LOADp);
    /*_p12.BEGE*/ wire BEGE_SHIFTER_SETn_08 = nand2(ch1.DYGY_CH1_FREQ08.qp_new(), KEDO_SHIFTER_LOADp);
    /*_p12.BESO*/ wire BESO_SHIFTER_SETn_09 = nand2(ch1.EVAB_CH1_FREQ09.qp_new(), KEDO_SHIFTER_LOADp);
    /*_p12.BEJU*/ wire BEJU_SHIFTER_SETn_10 = nand2(ch1.AXAN_CH1_FREQ10.qp_new(), KEDO_SHIFTER_LOADp);

    /*_p12.JUTA*/ wire JUTA_CH1_FREQ00n = not1(ch1.HYKA_CH1_FREQ00.qp_new());
    /*_p12.HOXE*/ wire HOXE_CH1_FREQ01n = not1(ch1.JYKA_CH1_FREQ01.qp_new());
    /*_p12.HUNY*/ wire HUNY_CH1_FREQ02n = not1(ch1.HAVO_CH1_FREQ02.qp_new());
    /*_p12.KEKE*/ wire KEKE_CH1_FREQ03n = not1(ch1.EDUL_CH1_FREQ03.qp_new());
    /*_p12.KOVO*/ wire KOVO_CH1_FREQ04n = not1(ch1.FELY_CH1_FREQ04.qp_new());
    /*_p12.FOHY*/ wire FOHY_CH1_FREQ05n = not1(ch1.HOLU_CH1_FREQ05.qp_new());
    /*_p12.DEBO*/ wire DEBO_CH1_FREQ06n = not1(ch1.HYXU_CH1_FREQ06.qp_new());
    /*_p12.CYKY*/ wire CYKY_CH1_FREQ07n = not1(ch1.HOPO_CH1_FREQ07.qp_new());
    /*_p12.BYRU*/ wire BYRU_CH1_FREQ08n = not1(ch1.DYGY_CH1_FREQ08.qp_new());
    /*_p12.APAT*/ wire APAT_CH1_FREQ09n = not1(ch1.EVAB_CH1_FREQ09.qp_new());
    /*_p12.BOJO*/ wire BOJO_CH1_FREQ10n = not1(ch1.AXAN_CH1_FREQ10.qp_new());

    /*_p12.KOKO*/ wire KOKO_CH1_FREQ00n = and2(JUTA_CH1_FREQ00n, KAPE_SHIFTER_LOADp);
    /*_p12.JOCY*/ wire JOCY_CH1_FREQ01n = and2(HOXE_CH1_FREQ01n, KAPE_SHIFTER_LOADp);
    /*_p12.JEHY*/ wire JEHY_CH1_FREQ02n = and2(HUNY_CH1_FREQ02n, KAPE_SHIFTER_LOADp);
    /*_p12.KAXY*/ wire KAXY_CH1_FREQ03n = and2(KEKE_CH1_FREQ03n, JUJU_SHIFTER_LOADp);
    /*_p12.KEVY*/ wire KEVY_CH1_FREQ04n = and2(KOVO_CH1_FREQ04n, JUJU_SHIFTER_LOADp);
    /*_p12.EVOF*/ wire EVOF_CH1_FREQ05n = and2(FOHY_CH1_FREQ05n, JUJU_SHIFTER_LOADp);
    /*_p12.EREG*/ wire EREG_CH1_FREQ06n = and2(DEBO_CH1_FREQ06n, JUJU_SHIFTER_LOADp);
    /*_p12.BAPU*/ wire BAPU_CH1_FREQ07n = and2(CYKY_CH1_FREQ07n, KEDO_SHIFTER_LOADp);
    /*_p12.AFUG*/ wire AFUG_CH1_FREQ08n = and2(BYRU_CH1_FREQ08n, KEDO_SHIFTER_LOADp);
    /*_p12.BUVO*/ wire BUVO_CH1_FREQ09n = and2(APAT_CH1_FREQ09n, KEDO_SHIFTER_LOADp);
    /*_p12.AFYR*/ wire AFYR_CH1_FREQ10n = and2(BOJO_CH1_FREQ10n, KEDO_SHIFTER_LOADp);

    /*_p12.JADO*/ wire JADO_SHIFTER_RSTn_00 = nor2(KEBA_APU_RSTp(), KOKO_CH1_FREQ00n);
    /*_p12.HOBU*/ wire HOBU_SHIFTER_RSTn_01 = nor2(KEBA_APU_RSTp(), JOCY_CH1_FREQ01n);
    /*_p12.HYVU*/ wire HYVU_SHIFTER_RSTn_02 = nor2(KEBA_APU_RSTp(), JEHY_CH1_FREQ02n);
    /*_p12.KETO*/ wire KETO_SHIFTER_RSTn_03 = nor2(KEBA_APU_RSTp(), KAXY_CH1_FREQ03n);
    /*_p12.KYBO*/ wire KYBO_SHIFTER_RSTn_04 = nor2(KEBA_APU_RSTp(), KEVY_CH1_FREQ04n);
    /*_p12.EZUK*/ wire EZUK_SHIFTER_RSTn_05 = nor2(KEBA_APU_RSTp(), EVOF_CH1_FREQ05n);
    /*_p12.ENOK*/ wire ENOK_SHIFTER_RSTn_06 = nor2(KEBA_APU_RSTp(), EREG_CH1_FREQ06n);
    /*_p12.BEWO*/ wire BEWO_SHIFTER_RSTn_07 = nor2(KEBA_APU_RSTp(), BAPU_CH1_FREQ07n);
    /*_p12.AGOR*/ wire AGOR_SHIFTER_RSTn_08 = nor2(KEBA_APU_RSTp(), AFUG_CH1_FREQ08n);
    /*_p12.AFUX*/ wire AFUX_SHIFTER_RSTn_09 = nor2(KEBA_APU_RSTp(), BUVO_CH1_FREQ09n);
    /*_p12.AVUF*/ wire AVUF_SHIFTER_RSTn_10 = nor2(KEBA_APU_RSTp(), AFYR_CH1_FREQ10n);

    /*#p12.FAJA*/ wire FAJA_SHIFT_CLK = not1(EGOR_SHIFT_CLK);
    /*#p12.EJYB*/ wire EJYB_SHIFT_CLK = not1(FAJA_SHIFT_CLK);
    /*#p12.CYBE*/ wire CYBE_SHIFT_CLK = not1(EJYB_SHIFT_CLK);
    /*#p12.BECY*/ wire BECY_SHIFT_CLK = not1(CYBE_SHIFT_CLK);

    /*#p12.FABU*/ ch1.FABU_CH1_SHIFT00.dff22(EGOR_SHIFT_CLK, HOZU_SHIFTER_SETn_00, JADO_SHIFTER_RSTn_00, ch1.JEFA_CH1_SHIFT01.qp_old());
    /*_p12.JEFA*/ ch1.JEFA_CH1_SHIFT01.dff22(EGOR_SHIFT_CLK, HOLA_SHIFTER_SETn_01, HOBU_SHIFTER_RSTn_01, ch1.GOGA_CH1_SHIFT02.qp_old());
    /*_p12.GOGA*/ ch1.GOGA_CH1_SHIFT02.dff22(EGOR_SHIFT_CLK, HAWY_SHIFTER_SETn_02, HYVU_SHIFTER_RSTn_02, ch1.JOLU_CH1_SHIFT03.qp_old());
    /*_p12.JOLU*/ ch1.JOLU_CH1_SHIFT03.dff22(EJYB_SHIFT_CLK, KYRY_SHIFTER_SETn_03, KETO_SHIFTER_RSTn_03, ch1.JOTA_CH1_SHIFT04.qp_old());
    /*_p12.JOTA*/ ch1.JOTA_CH1_SHIFT04.dff22(EJYB_SHIFT_CLK, KOLA_SHIFTER_SETn_04, KYBO_SHIFTER_RSTn_04, ch1.FUDE_CH1_SHIFT05.qp_old());
    /*_p12.FUDE*/ ch1.FUDE_CH1_SHIFT05.dff22(EJYB_SHIFT_CLK, GOVO_SHIFTER_SETn_05, EZUK_SHIFTER_RSTn_05, ch1.FEDO_CH1_SHIFT06.qp_old());
    /*_p12.FEDO*/ ch1.FEDO_CH1_SHIFT06.dff22(EJYB_SHIFT_CLK, EKEM_SHIFTER_SETn_06, ENOK_SHIFTER_RSTn_06, ch1.EXAC_CH1_SHIFT07.qp_old());
    /*_p12.EXAC*/ ch1.EXAC_CH1_SHIFT07.dff22(BECY_SHIFT_CLK, DACE_SHIFTER_SETn_07, BEWO_SHIFTER_RSTn_07, ch1.ELUX_CH1_SHIFT08.qp_old());
    /*_p12.ELUX*/ ch1.ELUX_CH1_SHIFT08.dff22(BECY_SHIFT_CLK, BEGE_SHIFTER_SETn_08, AGOR_SHIFTER_RSTn_08, ch1.AGEZ_CH1_SHIFT09.qp_old());
    /*_p12.AGEZ*/ ch1.AGEZ_CH1_SHIFT09.dff22(BECY_SHIFT_CLK, BESO_SHIFTER_SETn_09, AFUX_SHIFTER_RSTn_09, ch1.BEKU_CH1_SHIFT10.qp_old());
    /*_p12.BEKU*/ ch1.BEKU_CH1_SHIFT10.dff22(BECY_SHIFT_CLK, BEJU_SHIFTER_SETn_10, AVUF_SHIFTER_RSTn_10, 0);
  }

  //----------
  // Restart sequencer

  {
    {
      /*#p13.DUKA*/ wire DUKA_APU_RSTn = not1(KEBA_APU_RSTp());
      /*#p13.EZEC*/ ch1.EZEC_CH1_TRIGn.dff17(DOVA_ABCDxxxx(), DUKA_APU_RSTn, ch1.DUPE_NR14_TRIG.qn_new());
    }

    /*#p13.EGET*/ wire EGET_TRIG_RSTn = nor2(KEBA_APU_RSTp(), ch1.FARE_CH1_TRIGp.qp_new());

    {
      /*#p13.GEFE*/ wire GEFE_TRIG_RSTp = not1(EGET_TRIG_RSTn);
      /*#p13.FYFO*/ ch1.FYFO_CH1_TRIGn.nor_latch(GEFE_TRIG_RSTp, ch1.EZEC_CH1_TRIGn.qp_new());
    }

    {
      /*#p13.ERUM*/ wire ERUM_RSTn = not1(KEBA_APU_RSTp());
      /*#p13.FEKU*/ ch1.FEKU_CH1_TRIGp.dff17(DYFA_CLK_1M, EGET_TRIG_RSTn, ch1.FYFO_CH1_TRIGn.qn_old()); // schematic wrong?
      /*#p13.FARE*/ ch1.FARE_CH1_TRIGp.dff17(DYFA_CLK_1M, ERUM_RSTn,      ch1.FEKU_CH1_TRIGp.qp_old());
      /*#p13.FYTE*/ ch1.FYTE_CH1_TRIGp.dff17(DYFA_CLK_1M, ERUM_RSTn,      ch1.FARE_CH1_TRIGp.qp_old());
    }
  }

  //----------
  // Waveform generator

  {
    {
      /*#p13.FEMY*/ wire FEMY_CH2_OFFn = nor2(HOCA_CH1_AMP_ENn, KEBA_APU_RSTp());
      /*#p13.GEPU*/ wire GEPU_CH1_TRIGn = not1(ch1.FYTE_CH1_TRIGp.qp_new());
      /*#p13.GEXU*/ ch1.GEXU_FREQ_GATE.nand_latch(FEMY_CH2_OFFn, GEPU_CH1_TRIGn);
    }

    {
      /*#p13.CALA*/ wire CALA_FREQ_OVERFLOWp = not1(ch1.COPU_CH1_FREQ_CNT_10.qp_new());
      /*#p13.DOKA*/ wire DOKA_COMY_RSTp = and2(ch1.COMY_FREQ_RSTp.qp_new(), DYFA_CLK_1M);
      /*#p13.DYRU*/ wire DYRU_COMY_RSTn = nor3(KEBA_APU_RSTp(), ch1.FEKU_CH1_TRIGp.qp_new(), DOKA_COMY_RSTp);
      /*_p13.COMY*/ ch1.COMY_FREQ_RSTp.dff17(CALA_FREQ_OVERFLOWp, DYRU_COMY_RSTn, ch1.COMY_FREQ_RSTp.qn_old());
    }

    /*#p13.CYTE*/ wire CYTE_FREQ_RSTn = not1(ch1.COMY_FREQ_RSTp.qp_new());
    /*#p13.COPE*/ wire COPE_FREQ_RSTp = not1(CYTE_FREQ_RSTn);
    /*#p11.EPYK*/ wire EPYK_FREQ_RSTn = nor2(COPE_FREQ_RSTp, ch1.FEKU_CH1_TRIGp.qp_new());

    {
      /*#p11.FULO*/ wire FULO_FREQ_CLK = nor2(DYFA_CLK_1M, ch1.GEXU_FREQ_GATE.qp_new());
      /*#p11.GEKU*/ wire GEKU_FREQ_CLK = not1(FULO_FREQ_CLK);
      /*#p11.FUME*/ wire FUME_FREQ_RSTa = not1(EPYK_FREQ_RSTn);
      /*#p11.GAXE*/ ch1.GAXE_CH1_FREQ_CNT_00.dff20(GEKU_FREQ_CLK,                     FUME_FREQ_RSTa, ch1.HYKA_CH1_FREQ00.qp_new());
      /*#p11.HYFE*/ ch1.HYFE_CH1_FREQ_CNT_01.dff20(ch1.GAXE_CH1_FREQ_CNT_00.qp_new(), FUME_FREQ_RSTa, ch1.JYKA_CH1_FREQ01.qp_new());
      /*_p11.JYTY*/ ch1.JYTY_CH1_FREQ_CNT_02.dff20(ch1.HYFE_CH1_FREQ_CNT_01.qp_new(), FUME_FREQ_RSTa, ch1.HAVO_CH1_FREQ02.qp_new());
      /*_p11.KYNA*/ ch1.KYNA_CH1_FREQ_CNT_03.dff20(ch1.JYTY_CH1_FREQ_CNT_02.qp_new(), FUME_FREQ_RSTa, ch1.EDUL_CH1_FREQ03.qp_new());
    }

    {
      /*#p11.KYPE*/ wire KYPE_FREQ_CLK = not1(ch1.KYNA_CH1_FREQ_CNT_03.qn_new());
      /*#p11.DEGA*/ wire DEGA_FREQ_RSTb = not1(EPYK_FREQ_RSTn);
      /*_p11.JEMA*/ ch1.JEMA_CH1_FREQ_CNT_04.dff20(KYPE_FREQ_CLK,                     DEGA_FREQ_RSTb, ch1.FELY_CH1_FREQ04.qp_new());
      /*_p11.HYKE*/ ch1.HYKE_CH1_FREQ_CNT_05.dff20(ch1.JEMA_CH1_FREQ_CNT_04.qp_new(), DEGA_FREQ_RSTb, ch1.HOLU_CH1_FREQ05.qp_new());
      /*_p11.FEVA*/ ch1.FEVA_CH1_FREQ_CNT_06.dff20(ch1.HYKE_CH1_FREQ_CNT_05.qp_new(), DEGA_FREQ_RSTb, ch1.HYXU_CH1_FREQ06.qp_new());
      /*_p11.EKOV*/ ch1.EKOV_CH1_FREQ_CNT_07.dff20(ch1.FEVA_CH1_FREQ_CNT_06.qp_new(), DEGA_FREQ_RSTb, ch1.HOPO_CH1_FREQ07.qp_new());
    }

    {
      /*#p11.DERU*/ wire DERU_FREQ_CLK = not1(ch1.EKOV_CH1_FREQ_CNT_07.qn_new());
      /*#p11.DAKO*/ wire DAKO_FREQ_RSTc = not1(EPYK_FREQ_RSTn);
      /*_p11.EMUS*/ ch1.EMUS_CH1_FREQ_CNT_08.dff20(DERU_FREQ_CLK,                     DAKO_FREQ_RSTc, ch1.DYGY_CH1_FREQ08.qp_new());

      // FIXME these clocks have to be wrong
      /*_p11.EVAK*/ ch1.EVAK_CH1_FREQ_CNT_09.dff20(ch1.EVAK_CH1_FREQ_CNT_09.qp_new(), DAKO_FREQ_RSTc, ch1.EVAB_CH1_FREQ09.qp_new());
      /*_p11.COPU*/ ch1.COPU_CH1_FREQ_CNT_10.dff20(ch1.COPU_CH1_FREQ_CNT_10.qp_new(), DAKO_FREQ_RSTc, ch1.AXAN_CH1_FREQ10.qp_new());
    }

    {
      /*#p13.DAJO*/ wire DAJO_PHASE_CLKa = not1(COPE_FREQ_RSTp);
      /*#p13.ESUT*/ ch1.ESUT_PHASE_xBxDxFxH.dff17(DAJO_PHASE_CLKa,                  CEPO_APU_RSTn(), ch1.ESUT_PHASE_xBxDxFxH.qn_old());
      /*#p13.EROS*/ ch1.EROS_PHASE_xxCDxxGH.dff13(ch1.ESUT_PHASE_xBxDxFxH.qn_new(), CEPO_APU_RSTn(), ch1.EROS_PHASE_xxCDxxGH.qn_old());
      /*#p13.DAPE*/ ch1.DAPE_PHASE_xxxxEFGH.dff13(ch1.EROS_PHASE_xxCDxxGH.qn_new(), CEPO_APU_RSTn(), ch1.DAPE_PHASE_xxxxEFGH.qn_old());
    }

    {
      /*#p13.DUVO*/ wire DUVO_PHASE_AxCxExGx = not1(ch1.ESUT_PHASE_xBxDxFxH.qp_new());
      /*#p13.EZOZ*/ wire EZOZ_PHASE_xxxxxxGH = and2(ch1.DAPE_PHASE_xxxxEFGH.qp_new(), ch1.EROS_PHASE_xxCDxxGH.qp_new());
      /*#p13.ENEK*/ wire ENEK_PHASE_xxxxxxGx = and2(EZOZ_PHASE_xxxxxxGH, DUVO_PHASE_AxCxExGx);
      /*#p13.CODO*/ wire CODO_PHASE_ABCDEFxx = not1(EZOZ_PHASE_xxxxxxGH);

      /*#p13.COSO*/ wire COSO = nor2(ch1.CENA_NR11_DUTY_0.qn_new(), ch1.DYCA_NR11_DUTY_1.qn_new());
      /*#p13.CAVA*/ wire CAVA = nor2(ch1.CENA_NR11_DUTY_0.qp_new(), ch1.DYCA_NR11_DUTY_1.qn_new());
      /*#p13.CEVU*/ wire CEVU = nor2(ch1.CENA_NR11_DUTY_0.qn_new(), ch1.DYCA_NR11_DUTY_1.qp_new());
      /*#p13.CAXO*/ wire CAXO = nor2(ch1.CENA_NR11_DUTY_0.qp_new(), ch1.DYCA_NR11_DUTY_1.qp_new());
      /*#p13.DUNA*/ wire DUNA_RAW_BIT = amux4(ENEK_PHASE_xxxxxxGx, COSO,
                                              EZOZ_PHASE_xxxxxxGH, CAVA,
                                              ch1.DAPE_PHASE_xxxxEFGH.qp_new(), CEVU,
                                              CODO_PHASE_ABCDEFxx, CAXO);
      /*_p13.DUWO*/ ch1.DUWO_RAW_BIT_SYNCp.dff17(COPE_FREQ_RSTp, CEPO_APU_RSTn(), DUNA_RAW_BIT);
    }
  }

  //----------
  // Envelope generator. I'm not entirely sure how this works...

  {
    {
      /*#p13.JONE*/ wire JONE_CLK_128p = not1(BYFE_CLK_128n());
      /*#p13.KADO*/ wire KADO_APU_RSTn = not1(KEBA_APU_RSTp());
      /*#p13.KALY*/ ch1.KALY_ENV_DELAY_CLK_64n.dff17(JONE_CLK_128p, KADO_APU_RSTn, ch1.KALY_ENV_DELAY_CLK_64n.qn_old());
      /*#p13.KERE*/ wire KERE_ENV_DELAY_CLK_64p = not1(ch1.KALY_ENV_DELAY_CLK_64n.qp_new());
      /*#p13.JOLA*/ wire JOLA_ENV_DELAY_CLK_64n  = not1(KERE_ENV_DELAY_CLK_64p);

      /*#p13.KAZA*/ wire KAZA_ENV_DELAY_LOADn = nor2(ch1.FEKU_CH1_TRIGp.qp_new(), ch1.KOZY_ENV_CLKp.qp_new());
      /*#p13.KUXU*/ wire KUXU_ENV_DELAY_LOADp = not1(KAZA_ENV_DELAY_LOADn);

      /*#p13.JOVA*/ ch1.JOVA_ENV_DELAY0p.dff20(JOLA_ENV_DELAY_CLK_64n,        KUXU_ENV_DELAY_LOADp, ch1.JUSA_NR12_DELAY0n.qp_old());
      /*#p13.KENU*/ ch1.KENU_ENV_DELAY1p.dff20(ch1.JOVA_ENV_DELAY0p.qp_new(), KUXU_ENV_DELAY_LOADp, ch1.JUZY_NR12_DELAY1n.qp_old());
      /*#p13.KERA*/ ch1.KERA_ENV_DELAY2p.dff20(ch1.KENU_ENV_DELAY1p.qp_new(), KUXU_ENV_DELAY_LOADp, ch1.JOMA_NR12_DELAY2n.qp_old());
    }

    // so this implies that NR12_DELAY* is DFFn?
    /*#p13.KOMA*/ wire KOMA_ENV_OFFp = nor3(ch1.JUSA_NR12_DELAY0n.qn_new(), ch1.JUZY_NR12_DELAY1n.qn_new(), ch1.JOMA_NR12_DELAY2n.qn_new());

    {
      /*#p13.KURY*/ wire KURY_ENV_CLKn = not1(ch1.KOZY_ENV_CLKp.qp_new());
      /*#p13.KUKU*/ wire KUKU_ENV_CLKp = nor2(HORU_CLK_512p(), KURY_ENV_CLKn);
      /*#p13.KORO*/ wire KORO_ENV_CLK_RSTn = nor4(KUKU_ENV_CLKp, KOMA_ENV_OFFp, ch1.FEKU_CH1_TRIGp.qp_new(), KEBA_APU_RSTp());
      /*#p13.KOTE*/ wire KOTE_ENV_DELAY_DONEp = and3(ch1.KERA_ENV_DELAY2p.qp_new(), ch1.KENU_ENV_DELAY1p.qp_new(), ch1.JOVA_ENV_DELAY0p.qp_new());
      /*#p13.KOZY*/ ch1.KOZY_ENV_CLKp.dff17(HORU_CLK_512p(), KORO_ENV_CLK_RSTn, KOTE_ENV_DELAY_DONEp);
    }

    {
      /*#p13.KORU*/ wire KORU = nor2(ch1.FEKU_CH1_TRIGp.qp_new(), KEBA_APU_RSTp());
      /*#p13.HUFU*/ wire HUFU = nand5(ch1.JAFY_NR12_ENV_DIR.qp_new(), ch1.HAFO_CH1_ENV0p.qp_new(), ch1.HEMY_CH1_ENV1p.qp_new(), ch1.HOKO_CH1_ENV2p.qp_new(), ch1.HEVO_CH1_ENV3p.qp_new());
      /*#p13.HAKE*/ wire HAKE_ENV_TOP = not1(HUFU);
      /*#p13.HANO*/ wire HANO_ENV_BOT = nor5(ch1.JAFY_NR12_ENV_DIR.qp_new(), ch1.HAFO_CH1_ENV0p.qp_new(), ch1.HEMY_CH1_ENV1p.qp_new(), ch1.HOKO_CH1_ENV2p.qp_new(), ch1.HEVO_CH1_ENV3p.qp_new());
      /*#p13.JADE*/ wire JADE_ENV_MAX = or2(HANO_ENV_BOT, HAKE_ENV_TOP);
      /*#p13.KYNO*/ ch1.KYNO_ENV_STOP.dff17(ch1.KOZY_ENV_CLKp.qp_new(), KORU, JADE_ENV_MAX);
    }

    {
      /*#p13.KEKO*/ wire KEKO_ENV_STARTp = or2(KEBA_APU_RSTp(), ch1.FEKU_CH1_TRIGp.qp_new());
      /*#p13.KEZU*/ ch1.KEZU_ENV_ACTIVE.nor_latch(ch1.KYNO_ENV_STOP.qp_new(), KEKO_ENV_STARTp);
    }

    {
      /*#p13.KAKE*/ wire KAKE = and3(ch1.KOZY_ENV_CLKp.qp_new(), KOMA_ENV_OFFp, ch1.KEZU_ENV_ACTIVE.qp_new());

      /*#p13.JUFY*/ wire JUFY_ENV_DELTA0 = amux2(ch1.JAFY_NR12_ENV_DIR.qn_new(), KAKE,                        KAKE,                      ch1.JAFY_NR12_ENV_DIR.qp_new());
      /*#p13.HYTO*/ wire HYTO_ENV_DELTA1 = amux2(ch1.JAFY_NR12_ENV_DIR.qn_new(), ch1.HAFO_CH1_ENV0p.qp_new(), ch1.HAFO_CH1_ENV0p.qn_new(), ch1.JAFY_NR12_ENV_DIR.qp_new());
      /*#p13.HETO*/ wire HETO_ENV_DELTA2 = amux2(ch1.JAFY_NR12_ENV_DIR.qn_new(), ch1.HEMY_CH1_ENV1p.qp_new(), ch1.HEMY_CH1_ENV1p.qn_new(), ch1.JAFY_NR12_ENV_DIR.qp_new());
      /*#p13.HESU*/ wire HESU_ENV_DELTA3 = amux2(ch1.JAFY_NR12_ENV_DIR.qn_new(), ch1.HOKO_CH1_ENV2p.qp_new(), ch1.HOKO_CH1_ENV2p.qn_new(), ch1.JAFY_NR12_ENV_DIR.qp_new());

      // these were connected wrong in schematic
      /*#p13.HEVO*/ ch1.HEVO_CH1_ENV3p.dff20(HESU_ENV_DELTA3, ch1.FEKU_CH1_TRIGp.qp_new(), ch1.JOPU_NR12_VOL3.qn_old());
      /*#p13.HOKO*/ ch1.HOKO_CH1_ENV2p.dff20(HETO_ENV_DELTA2, ch1.FEKU_CH1_TRIGp.qp_new(), ch1.JENA_NR12_VOL2.qn_old());
      /*#p13.HEMY*/ ch1.HEMY_CH1_ENV1p.dff20(HYTO_ENV_DELTA1, ch1.FEKU_CH1_TRIGp.qp_new(), ch1.JAXO_NR12_VOL1.qn_old());
      /*#p13.HAFO*/ ch1.HAFO_CH1_ENV0p.dff20(JUFY_ENV_DELTA0, ch1.FEKU_CH1_TRIGp.qp_new(), ch1.JATY_NR12_VOL0.qn_old());
    }

    //----------
    // Audio output

    {
      /*#p13.BONE*/ wire BONE_FREQ_OVERFLOWp = not1(ATYS_FREQ_OVERFLOW);
      /*#p13.CYFA*/ wire CYFA_LEN_OVERp = and2(ch1.CERO_CH1_LEN_DONE.qp_new(), ch1.BOKO_NR14_LEN_EN.qn_new());
      /*#p13.BERY*/ wire BERY_CH1_STOPp = or4(BONE_FREQ_OVERFLOWp, KEBA_APU_RSTp(), CYFA_LEN_OVERp, HOCA_CH1_AMP_ENn);

      /*#p13.CYTO*/ ch1.CYTO_CH1_ACTIVEp.nor_latch(ch1.FEKU_CH1_TRIGp.qp_new(), BERY_CH1_STOPp);

      /*#p13.COWE*/ wire COWE_BIT_OUTp = and2(ch1.CYTO_CH1_ACTIVEp.qp_new(), ch1.DUWO_RAW_BIT_SYNCp.qp_new());
      /*#p13.BOTO*/ wire BOTO_BIT_OUTp = or2(COWE_BIT_OUTp, EDEK_NR52_DBG_APUn);
      /*#p13.AMOP*/ wire AMOP_CH1_OUT0 = and2(ch1.HAFO_CH1_ENV0p.qp_new(), BOTO_BIT_OUTp);
      /*#p13.ASON*/ wire ASON_CH1_OUT1 = and2(ch1.HEMY_CH1_ENV1p.qp_new(), BOTO_BIT_OUTp);
      /*#p13.AGOF*/ wire AGOF_CH1_OUT2 = and2(ch1.HOKO_CH1_ENV2p.qp_new(), BOTO_BIT_OUTp);
      /*#p13.ACEG*/ wire ACEG_CH1_OUT3 = and2(ch1.HEVO_CH1_ENV3p.qp_new(), BOTO_BIT_OUTp);
    }
  }














  /*_p10.TACE*/ wire TACE_AMP_ENn = and4(HOCA_CH1_AMP_ENn, FUTE_CH2_AMP_ENn, ch3.GUXE_CH3_AMP_ENn.qp_new(), GEVY_CH4_AMP_ENn);
}

//-----------------------------------------------------------------------------
