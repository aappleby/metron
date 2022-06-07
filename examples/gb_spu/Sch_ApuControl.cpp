/// plait_noparse

#include "Sch_ApuControl.h"


//-----------------------------------------------------------------------------

void ApuControl::tick() {
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

  wire ADDR_FFXX = 0;
  wire UNOR_MODE_DBG2 = 0;
  wire ALUR_SYS_RSTn = true;
  wire TEDO_CPU_RDp = false;
  wire TAPU_CPU_WRp = false;
  wire AZOF_xBxDxFxH = 0;
  wire BUDE_xxxxEFGH = 0;
  wire UMER_DIV10n_old = 0;

  // SPU clock tree

  /*_p01.ATAG*/ wire ATAG_AxCxExGx = not1(AZOF_xBxDxFxH);
  /*_p01.AMUK*/ wire AMUK_xBxDxFxH = not1(ATAG_AxCxExGx);
  /*_p01.APUV*/ wire APUV_AxCxExGx = not1(AMUK_xBxDxFxH);
  /*_p01.CYBO*/ wire CYBO_AxCxExGx = not1(AMUK_xBxDxFxH);
  /*_p01.ARYF*/ wire ARYF_AxCxExGx = not1(AMUK_xBxDxFxH);

  /*_p01.DOVA*/ wire DOVA_ABCDxxxx = not1(BUDE_xxxxEFGH);

  // SPU reset tree

  /*_p09.HAPO*/ wire HAPO_SYS_RSTp = not1(ALUR_SYS_RSTn);
  /*_p09.GUFO*/ wire GUFO_SYS_RSTn = not1(HAPO_SYS_RSTp);


  /*_p09.JYRO*/ wire JYRO_APU_RSTp = or2(HAPO_SYS_RSTp, HADA_NR52_ALL_SOUND_ON.qn_new());
  /*_p09.KEPY*/ wire KEPY_APU_RSTn = not1(JYRO_APU_RSTp);
  /*_p09.KUBY*/ wire KUBY_APU_RSTn = not1(JYRO_APU_RSTp);
  /*_p09.KEBA*/ wire KEBA_APU_RSTp = not1(KUBY_APU_RSTn);
  /*_p01.BOPO*/ wire BOPO_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p01.BELA*/ wire BELA_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p09.ATYV*/ wire ATYV_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p09.KAME*/ wire KAME_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p01.ATUS*/ wire ATUS_APU_RSTn = not1(KEBA_APU_RSTp);


  // CPU read/write signals

  /*_p09.AGUZ*/ wire AGUZ_CPU_RDn = not1(TEDO_CPU_RDp);
  /*_p10.BAFU*/ wire BAFU_CPU_WRn = not1(TAPU_CPU_WRp);
  /*_p10.BOGY*/ wire BOGY_CPU_WRp = not1(BAFU_CPU_WRn);

  // SPU clock dividers

  /*_p01.CERY*/ CERY_2M.dff17(CYBO_AxCxExGx,    BELA_APU_RSTn, CERY_2M.qn_old());
  /*_p01.ATYK*/ ATYK_2M.dff17(ARYF_AxCxExGx,    BOPO_APU_RSTn, ATYK_2M.qn_old());
  /*_p09.AJER*/ AJER_2M.dff17(APUV_AxCxExGx,    ATYV_APU_RSTn, AJER_2M.qn_old());

  /*_p01.AVOK*/ AVOK_1M.dff17(ATYK_2M.qn_new(), BOPO_APU_RSTn, AVOK_1M.qn_old());

  /*_p01.BAVU*/ wire BAVU_1M = not1(AVOK_1M.qp_new());

  /*_p01.JESO*/ JESO_CLK_512K.dff17(BAVU_1M, KAME_APU_RSTn, JESO_CLK_512K.qn_old());
  /*_p01.HAMA*/ wire HAMA_CLK_512K = not1(JESO_CLK_512K.qp_new()); // checkme

  // Low-speed clocks are picked up from DIV

  /*_p01.COKE*/ wire COKE_2M = not1(AJER_2M.qn_new());
  /*_p01.BARA*/ BARA_CLK_512.dff17(COKE_2M, ATUS_APU_RSTn, UMER_DIV10n_old);
  /*_p01.BURE*/ wire BURE_CLK_512 = not1(BARA_CLK_512.qp_new());
  /*_p01.FYNE*/ wire FYNE_CLK_512 = not1(BURE_CLK_512);

  /*_p01.CARU*/ CARU_CLK_256.dff17(BURE_CLK_512, ATUS_APU_RSTn, CARU_CLK_256.qn_old());
  /*_p01.CULO*/ wire CULO_CLK_256 = not1(CARU_CLK_256.qp_new());

  /*_p01.BYLU*/ BYLU_CLK_128.dff17(CARU_CLK_256.qn_new(), ATUS_APU_RSTn, BYLU_CLK_128.qn_old());
  /*_p01.APEF*/ wire APEF_CLK_128 = not1(BYLU_CLK_128.qp_new());

  /*_p01.GALE*/ wire GALE_CLK_512 = mux2p(FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K, FYNE_CLK_512);
  /*_p01.GEXY*/ wire GEXY_CLK_512 = not1(GALE_CLK_512);
  /*_p01.HORU*/ wire HORU_CLK_512 = not1(GEXY_CLK_512);

  /*_p01.BEZE*/ wire BEZE_CLK_256 = mux2p(FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K, CULO_CLK_256);
  /*_p01.COFU*/ wire COFU_CLK_256 = not1(BEZE_CLK_256);
  /*_p01.BUFY*/ wire BUFY_CLK_256 = not1(COFU_CLK_256);

  /*_p01.BULE*/ wire BULE_CLK_128 = mux2p(FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K, APEF_CLK_128);
  /*_p01.BARU*/ wire BARU_CLK_128 = not1(BULE_CLK_128);
  /*_p01.BYFE*/ wire BYFE_CLK_128 = not1(BARU_CLK_128);

  //---------
  // decodin'

  /*_p07.BAKO*/ wire BAKO_ADDR_FFXXn = not1(ADDR_FFXX);

  /*_p10.ATUP*/ wire ATUP_A04n = not1(BUS_CPU_A04p.out_new());
  /*_p10.BOXY*/ wire BOXY_A05n = not1(BUS_CPU_A05p.out_new());
  /*_p10.ASAD*/ wire ASAD_A06n = not1(BUS_CPU_A06p.out_new());
  /*_p10.AVUN*/ wire AVUN_A07n = not1(BUS_CPU_A07p.out_new());

  /*_p10.ATEG*/ wire ATEG_ADDR_XX1Xn = or4(BUS_CPU_A07p.out_new(), BUS_CPU_A06p.out_new(), BUS_CPU_A05p.out_new(), ATUP_A04n);
  /*_p10.AWET*/ wire AWET_ADDR_XX2Xn = or4(BUS_CPU_A07p.out_new(), BUS_CPU_A06p.out_new(), BOXY_A05n, BUS_CPU_A04p.out_new());

  /*_p10.BUNO*/ wire BUNO_ADDR_FF1Xp = nor2(BAKO_ADDR_FFXXn, ATEG_ADDR_XX1Xn);
  /*_p10.BANU*/ wire BANU_ADDR_FF1Xn = not1(BUNO_ADDR_FF1Xp);
  /*_p10.BEZY*/ wire BEZY_ADDR_FF2Xn = or2(AWET_ADDR_XX2Xn, BAKO_ADDR_FFXXn);

  /*_p10.DYTE*/ wire DYTE_ADDR_xxx0  = not1(BUS_CPU_A00p.out_new());
  /*_p10.AFOB*/ wire AFOB_ADDR_xx0x  = not1(BUS_CPU_A01p.out_new());
  /*_p10.ABUB*/ wire ABUB_ADDR_x0xx  = not1(BUS_CPU_A02p.out_new());
  /*_p10.ACOL*/ wire ACOL_ADDR_0xxx  = not1(BUS_CPU_A03p.out_new());

  /*_p10.DOSO*/ wire DOSO_ADDR_xxx1  = not1(DYTE_ADDR_xxx0);
  /*_p10.DUPA*/ wire DUPA_ADDR_xx1x  = not1(AFOB_ADDR_xx0x);
  /*_p10.DENO*/ wire DENO_ADDR_x1xx  = not1(ABUB_ADDR_x0xx);
  /*_p10.DUCE*/ wire DUCE_ADDR_1xxx  = not1(ACOL_ADDR_0xxx);

  /*_p10.DATU*/ wire DATU_ADDR_0100bn = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, AFOB_ADDR_xx0x, DYTE_ADDR_xxx0);
  /*_p10.CAFY*/ wire CAFY_ADDR_FF24   = nor2(BEZY_ADDR_FF2Xn, DATU_ADDR_0100bn);
  /*_p09.BYMA*/ wire BYMA_ADDR_FF24n  = not1(CAFY_ADDR_FF24);

  /*_p10.DURA*/ wire DURA_ADDR_0101n = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, AFOB_ADDR_xx0x, DOSO_ADDR_xxx1);
  /*_p10.CORA*/ wire CORA_ADDR_FF25p = nor2(BEZY_ADDR_FF2Xn, DURA_ADDR_0101n);
  /*_p09.GEPA*/ wire GEPA_ADDR_FF25n = not1(CORA_ADDR_FF25p);

  /*_p10.EKAG*/ wire EKAG_ADDR_0110n = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
  /*_p10.CONA*/ wire CONA_ADDR_FF2Xp = not1(BEZY_ADDR_FF2Xn);
  /*_p10.DOXY*/ wire DOXY_ADDR_FF26p = and2(CONA_ADDR_FF2Xp,  EKAG_ADDR_0110n); // something not right here

  //----------
  // FF24 NR50 write

  /*_p09.BOSU*/ wire BOSU_NR50_WRn = nand2(CAFY_ADDR_FF24, BOGY_CPU_WRp);
  /*_p09.BAXY*/ wire BAXY_NR50_WRp = not1(BOSU_NR50_WRn);
  /*_p09.BOWE*/ wire BOWE_NR50_WRp = not1(BOSU_NR50_WRn);
  /*_p09.BUBU*/ wire BUBU_NR50_WRn = not1(BAXY_NR50_WRp);
  /*_p09.ATAF*/ wire ATAF_NR50_WRn = not1(BOWE_NR50_WRp);

  /*_p09.APEG*/ APEG_NR50_VOL_L0   .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D00p.out_old());
  /*_p09.BYGA*/ BYGA_NR50_VOL_L1   .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D01p.out_old());
  /*_p09.AGER*/ AGER_NR50_VOL_L2   .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D02p.out_old());
  /*_p09.APOS*/ APOS_NR50_VIN_TO_L .dff9(ATAF_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D03p.out_old());
  /*_p09.BYRE*/ BYRE_NR50_VOL_R0   .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D04p.out_old());
  /*_p09.BUMO*/ BUMO_NR50_VOL_R1   .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D05p.out_old());
  /*_p09.COZU*/ COZU_NR50_VOL_R2   .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D06p.out_old());
  /*_p09.BEDU*/ BEDU_NR50_VIN_TO_R .dff9(BUBU_NR50_WRn, KEPY_APU_RSTn, BUS_CPU_D07p.out_old());

  //----------
  // FF25 NR51 write

  /*_p09.BUPO*/ wire BUPO_NR51_WRn = nand2(CORA_ADDR_FF25p, BOGY_CPU_WRp);
  /*_p09.BONO*/ wire BONO_NR51_WRp = not1(BUPO_NR51_WRn);
  /*_p09.BYFA*/ wire BYFA_NR51_WRp = not1(BUPO_NR51_WRn);

  /*_p09.ANEV*/ ANEV_NR51_D0.dff9(BONO_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D00p.out_old());
  /*_p09.BOGU*/ BOGU_NR51_D1.dff9(BONO_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D01p.out_old());
  /*_p09.BAFO*/ BAFO_NR51_D2.dff9(BONO_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D02p.out_old());
  /*_p09.ATUF*/ ATUF_NR51_D3.dff9(BONO_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D03p.out_old());
  /*_p09.BUME*/ BUME_NR51_D4.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D04p.out_old());
  /*_p09.BOFA*/ BOFA_NR51_D5.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D05p.out_old());
  /*_p09.BEFO*/ BEFO_NR51_D6.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D06p.out_old());
  /*_p09.BEPU*/ BEPU_NR51_D7.dff9(BYFA_NR51_WRp, KEPY_APU_RSTn, BUS_CPU_D07p.out_old());


#if 0

  // tace = and4(HOCA06, FUTE06, GUXE09, GEVY06)

  /*_p10.TACE*/ TACE_AMP_ENn = and2(HOCA_CH1_AMP_ENn, FUTE_CH2_AMP_ENn, GUXE_CH3_AMP_ENn, GEVY_CH4_AMP_ENn);

  ///*_p16.ANUJ*/ wire CPU_WR_WEIRD = and2(cpu_in.FROM_CPU5, APU_WR);
  #endif


  //----------
  // FF24 NR50

  /*_p09.BEFU*/ wire BEFU_NR50_RDp = nor2(AGUZ_CPU_RDn, BYMA_ADDR_FF24n);
  /*_p09.ADAK*/ wire ADAK_NR50_RDn = not1(BEFU_NR50_RDp);

  /*_p09.AKOD*/ triwire AKOD_D00 = tri6_nn(ADAK_NR50_RDn, APEG_NR50_VOL_L0.qp_new()); // polarity?
  /*_p09.AWED*/ triwire AWED_D01 = tri6_nn(ADAK_NR50_RDn, BYGA_NR50_VOL_L1.qp_new());
  /*_p09.AVUD*/ triwire AVUD_D02 = tri6_nn(ADAK_NR50_RDn, AGER_NR50_VOL_L2.qp_new());
  /*_p09.AXEM*/ triwire AXEM_D03 = tri6_nn(ADAK_NR50_RDn, APOS_NR50_VIN_TO_L.qp_new());
  /*_p09.AMAD*/ triwire AMAD_D04 = tri6_nn(ADAK_NR50_RDn, BYRE_NR50_VOL_R0.qp_new());
  /*_p09.ARUX*/ triwire ARUX_D05 = tri6_nn(ADAK_NR50_RDn, BUMO_NR50_VOL_R1.qp_new());
  /*_p09.BOCY*/ triwire BOCY_D06 = tri6_nn(ADAK_NR50_RDn, COZU_NR50_VOL_R2.qp_new());
  /*_p09.ATUM*/ triwire ATUR_D07 = tri6_nn(ADAK_NR50_RDn, BEDU_NR50_VIN_TO_R.qp_new());

  //----------
  // FF25 NR51 read

  /*_p09.HEFA*/ wire HEFA_NR51_RDp = nor2(GEPA_ADDR_FF25n, AGUZ_CPU_RDn); // outline wrong color in die
  /*_p09.GUMU*/ wire GUMU_NR51_RDn = not1(HEFA_NR51_RDp);

  /*_p09.BUZU*/ triwire BUZU_D0 = tri6_nn(GUMU_NR51_RDn, ANEV_NR51_D0.qn_new());
  /*_p09.CAPU*/ triwire CAPU_D1 = tri6_nn(GUMU_NR51_RDn, BOGU_NR51_D1.qn_new());
  /*_p09.CAGA*/ triwire CAGA_D2 = tri6_nn(GUMU_NR51_RDn, BAFO_NR51_D2.qn_new());
  /*_p09.BOCA*/ triwire BOCA_D3 = tri6_nn(GUMU_NR51_RDn, ATUF_NR51_D3.qn_new());
  /*_p09.CAVU*/ triwire CAVU_D4 = tri6_nn(GUMU_NR51_RDn, BUME_NR51_D4.qn_new());
  /*_p09.CUDU*/ triwire CUDU_D5 = tri6_nn(GUMU_NR51_RDn, BOFA_NR51_D5.qn_new());
  /*_p09.CADA*/ triwire CADA_D6 = tri6_nn(GUMU_NR51_RDn, BEFO_NR51_D6.qn_new());
  /*_p09.CERE*/ triwire CERE_D7 = tri6_nn(GUMU_NR51_RDn, BEPU_NR51_D7.qn_new());

  /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(BUZU_D0);
  /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(CAPU_D1);
  /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(CAGA_D2);
  /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(BOCA_D3);
  /*_BUS_CPU_D04p*/ BUS_CPU_D04p.tri_bus(CAVU_D4);
  /*_BUS_CPU_D05p*/ BUS_CPU_D05p.tri_bus(CUDU_D5);
  /*_BUS_CPU_D06p*/ BUS_CPU_D06p.tri_bus(CADA_D6);
  /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(CERE_D7);

  //----------
  // FF26 NR52 write

  /*_p09.ETUC*/ wire ETUC_NR52_WRp  = and2(DOXY_ADDR_FF26p, BOGY_CPU_WRp);
  /*_p09.HAWU*/ wire HAWU_NR52_WRn = nand2(DOXY_ADDR_FF26p, BOGY_CPU_WRp);
  /*_p09.BOPY*/ wire BOPY_NR52_WRn = nand2(DOXY_ADDR_FF26p, BOGY_CPU_WRp);
  /*_p09.FOKU*/ wire FOKU_NR52_WRn = not1(ETUC_NR52_WRp);

  /*_p09.EFOP*/ wire EFOP_NR52_DBG_APU_IN = and2(BUS_CPU_D04p.out_old(), UNOR_MODE_DBG2);

  /*_p09.FERO*/ FERO_NR52_DBG_APUn    .dff9 (FOKU_NR52_WRn, KEPY_APU_RSTn, EFOP_NR52_DBG_APU_IN);
  /*_p09.BOWY*/ BOWY_NR52_DBG_SWEEP   .dff17(BOPY_NR52_WRn, KEPY_APU_RSTn, BUS_CPU_D05p.out_old());
  /*_p09.HADA*/ HADA_NR52_ALL_SOUND_ON.dff17(HAWU_NR52_WRn, GUFO_SYS_RSTn, BUS_CPU_D07p.out_old()); // Since this bit controls APU_RESET*, it is reset by SYS_RESET.

  /*_p09.EDEK*/ wire EDEK_NR52_DBG_APUn = not1(FERO_NR52_DBG_APUn.qn_new());

  //----------
  // FF26 NR52 read

  /*_p18.COKA*/ wire COKA_CH3_ACTIVEp = not1(ch3.DAVO_CH3_ACTIVEn.qn_new());

  /*_p13.CARA*/ wire CARA_CH1_ACTIVEn = not1(ch1.CYTO_CH1_ACTIVE.qp_new());
  /*_p15.DEFU*/ wire DEFU_CH2_ACTIVEn = not1(ch2.DANE_CH2_ACTIVE.qp_new());
  /*_p18.ERED*/ wire ERED_CH3_ACTIVEn = not1(COKA_CH3_ACTIVEp);
  /*_p20.JUWA*/ wire JUWA_CH4_ACTIVEn = not1(ch4.GENA_CH4_ACTIVE.qp_new());

  /*_p09.CETO*/ wire CETO_CPU_RD = not1(AGUZ_CPU_RDn);
  /*_p09.KAZO*/ wire KAZO_CPU_RD = not1(AGUZ_CPU_RDn);
  /*_p09.CURU*/ wire CURU_CPU_RD = not1(AGUZ_CPU_RDn);
  /*_p09.GAXO*/ wire GAXO_CPU_RD = not1(AGUZ_CPU_RDn);
  /*_p09.KYDU*/ wire KYDU_CPU_RD = not1(AGUZ_CPU_RDn);

  /*_p09.DOLE*/ wire DOLE_NR52_RDn = nand2(DOXY_ADDR_FF26p, CETO_CPU_RD);
  /*_p09.KAMU*/ wire KAMU_NR52_RDn = nand2(DOXY_ADDR_FF26p, KAZO_CPU_RD);
  /*_p09.DURU*/ wire DURU_NR52_RDn = nand2(DOXY_ADDR_FF26p, CURU_CPU_RD);
  /*_p09.FEWA*/ wire FEWA_NR52_RDn = nand2(DOXY_ADDR_FF26p, GAXO_CPU_RD);
  /*_p09.JURE*/ wire JURE_NR52_RDn = nand2(DOXY_ADDR_FF26p, KYDU_CPU_RD);

  /*_p09.COTO*/ triwire COTO_D0 = tri6_nn(DOLE_NR52_RDn, CARA_CH1_ACTIVEn);
  /*_p09.EFUS*/ triwire EFUS_D1 = tri6_nn(DURU_NR52_RDn, DEFU_CH2_ACTIVEn);
  /*_p09.FATE*/ triwire FATE_D2 = tri6_nn(FEWA_NR52_RDn, ERED_CH3_ACTIVEn);
  /*_p09.KOGE*/ triwire KOGE_D3 = tri6_nn(KAMU_NR52_RDn, JUWA_CH4_ACTIVEn);
  /*_p09.HOPE*/ triwire HOPE_D7 = tri6_nn(JURE_NR52_RDn, HADA_NR52_ALL_SOUND_ON.qn_new());

  /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(COTO_D0);
  /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(EFUS_D1);
  /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(FATE_D2);
  /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(KOGE_D3);
  /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(HOPE_D7);
}
