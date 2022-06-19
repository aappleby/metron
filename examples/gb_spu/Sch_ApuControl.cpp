/// plait_noparse

#include "Sch_ApuControl.h"

//-----------------------------------------------------------------------------


void GBSound::tick() {
  wire SYKE_ADDR_FFXX = 0;
  wire UNOR_MODE_DBG2 = 0;
  wire ALUR_SYS_RSTn = true;
  wire TEDO_CPU_RDp = false;
  wire TAPU_CPU_WRp = false;
  wire AZOF_xBxDxFxH = 0;
  wire BUDE_xxxxEFGH = 0;
  wire UMER_DIV10n_old = 0;
  wire BUKE_ABxxxxxH = 0;
  wire SIG_CPU_HOLD_MEM = 0;
  wire SIG_IN_CPU_DBUS_FREE = 0;

  //----------
  // SPU clock tree

  /*_p01.ATAG*/ wire ATAG_AxCxExGx = not1(AZOF_xBxDxFxH);
  /*_p01.AMUK*/ wire AMUK_xBxDxFxH = not1(ATAG_AxCxExGx);
  /*_p01.APUV*/ wire APUV_AxCxExGx = not1(AMUK_xBxDxFxH);
  /*_p01.CYBO*/ wire CYBO_AxCxExGx = not1(AMUK_xBxDxFxH);
  /*_p01.ARYF*/ wire ARYF_AxCxExGx = not1(AMUK_xBxDxFxH);
  /*_p01.DOVA*/ wire DOVA_ABCDxxxx = not1(BUDE_xxxxEFGH);

  /*#p17.ABUR*/ wire ABUR_xxCDEFGx = not1(BUKE_ABxxxxxH);
  /*#p17.BORY*/ wire BORY_ABxxxxxH = not1(ABUR_xxCDEFGx);

  //----------
  // SPU reset tree

  /*_p09.HAPO*/ wire HAPO_SYS_RSTp = not1(ALUR_SYS_RSTn);
  /*_p09.GUFO*/ wire GUFO_SYS_RSTn = not1(HAPO_SYS_RSTp);

  /*_p09.JYRO*/ wire JYRO_APU_RSTp =  or2(HAPO_SYS_RSTp, HADA_NR52_ALL_SOUND_ON.qn_new());
  /*_p09.KEPY*/ wire KEPY_APU_RSTn = not1(JYRO_APU_RSTp);
  /*_p09.KUBY*/ wire KUBY_APU_RSTn = not1(JYRO_APU_RSTp);
  /*_p09.KEBA*/ wire KEBA_APU_RSTp = not1(KUBY_APU_RSTn);
  /*_p01.BOPO*/ wire BOPO_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p01.BELA*/ wire BELA_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p09.ATYV*/ wire ATYV_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p09.KAME*/ wire KAME_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p01.ATUS*/ wire ATUS_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p14.JYBU*/ wire JYBU_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p15.CEXE*/ wire CEXE_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p15.BUWE*/ wire BUWE_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p15.BYHO*/ wire BYHO_APU_RSTp = not1(BUWE_APU_RSTn);
  /*#p09.AFAT*/ wire AFAT_APU_RSTn = not1(KEBA_APU_RSTp);



  //----------
  // CPU read/write signals

  /*_p09.AGUZ*/ wire AGUZ_CPU_RDn = not1(TEDO_CPU_RDp);
  /*_p09.GAXO*/ wire GAXO_CPU_RDp = not1(AGUZ_CPU_RDn);
  /*_p14.FOGE*/ wire FOGE_CPU_RDp = not1(AGUZ_CPU_RDn);

  /*_p10.BAFU*/ wire BAFU_CPU_WRn = not1(TAPU_CPU_WRp);
  /*_p10.BOGY*/ wire BOGY_CPU_WRp = not1(BAFU_CPU_WRn);

  /*_p16.ANUJ*/ wire ANUJ_CPU_WR_WEIRD = and2(SIG_IN_CPU_DBUS_FREE, BOGY_CPU_WRp);

  /*_p09.EDEK*/ wire EDEK_NR52_DBG_APUn = not1(FERO_NR52_DBG_APUn.qn_new());
  /*_p14.FAPE*/ wire FAPE_DBG_CPU_RDp = and2(FOGE_CPU_RDp, EDEK_NR52_DBG_APUn);

  //----------
  // SPU clock dividers

  /*_p01.CERY*/ spu.CERY_2M.dff17(CYBO_AxCxExGx,    BELA_APU_RSTn, spu.CERY_2M.qn_old());
  /*_p01.ATYK*/ spu.ATYK_2M.dff17(ARYF_AxCxExGx,    BOPO_APU_RSTn, spu.ATYK_2M.qn_old());
  /*_p09.AJER*/ spu.AJER_2M.dff17(APUV_AxCxExGx,    ATYV_APU_RSTn, spu.AJER_2M.qn_old());
  /*_p01.COKE*/ wire COKE_2M = not1(spu.AJER_2M.qn_new());

  /*_p01.AVOK*/ spu.AVOK_1M.dff17(spu.ATYK_2M.qn_new(), BOPO_APU_RSTn, spu.AVOK_1M.qn_old());

  /*_p01.BAVU*/ wire BAVU_1M = not1(spu.AVOK_1M.qp_new());

  /*_p01.JESO*/ spu.JESO_CLK_512K.dff17(BAVU_1M, KAME_APU_RSTn, spu.JESO_CLK_512K.qn_old());
  /*_p01.HAMA*/ wire HAMA_CLK_512K = not1(spu.JESO_CLK_512K.qp_new()); // checkme

  /*_p15.AZEG*/ wire AZEG = not1(AMUK_xBxDxFxH);
  /*_p15.ATEP*/ ATEP_2MHZ.dff17(AZEG, BUWE_APU_RSTn, ATEP_2MHZ.qn_old());
  /*#p15.BUFO*/ wire BUFO = not1(ATEP_2MHZ.qp_new());
  /*#p15.CEMO*/ CEMO_1MHZ.dff17(BUFO, BYHO_APU_RSTp, CEMO_1MHZ.qn_old());

  //----------
  // Low-speed clocks are picked up from DIV

  /*_p01.BARA*/ spu.BARA_CLK_512.dff17(COKE_2M, ATUS_APU_RSTn, UMER_DIV10n_old);
  /*_p01.BURE*/ wire BURE_CLK_512 = not1(spu.BARA_CLK_512.qp_new());
  /*_p01.FYNE*/ wire FYNE_CLK_512 = not1(BURE_CLK_512);

  /*_p01.CARU*/ spu.CARU_CLK_256.dff17(BURE_CLK_512, ATUS_APU_RSTn, spu.CARU_CLK_256.qn_old());
  /*_p01.CULO*/ wire CULO_CLK_256 = not1(spu.CARU_CLK_256.qp_new());

  /*_p01.BYLU*/ spu.BYLU_CLK_128.dff17(spu.CARU_CLK_256.qn_new(), ATUS_APU_RSTn, spu.BYLU_CLK_128.qn_old());
  /*_p01.APEF*/ wire APEF_CLK_128 = not1(spu.BYLU_CLK_128.qp_new());

  /*_p01.GALE*/ wire GALE_CLK_512 = mux2p(FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K, FYNE_CLK_512);
  /*_p01.GEXY*/ wire GEXY_CLK_512 = not1(GALE_CLK_512);
  /*_p01.HORU*/ wire HORU_CLK_512 = not1(GEXY_CLK_512);

  /*_p01.BEZE*/ wire BEZE_CLK_256 = mux2p(FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K, CULO_CLK_256);
  /*_p01.COFU*/ wire COFU_CLK_256 = not1(BEZE_CLK_256);
  /*_p01.BUFY*/ wire BUFY_CLK_256 = not1(COFU_CLK_256);

  /*_p01.BULE*/ wire BULE_CLK_128 = mux2p(FERO_NR52_DBG_APUn.qp_new(), HAMA_CLK_512K, APEF_CLK_128);
  /*_p01.BARU*/ wire BARU_CLK_128 = not1(BULE_CLK_128);
  /*_p01.BYFE*/ wire BYFE_CLK_128 = not1(BARU_CLK_128);

  /*#p15.HOTA*/ wire HOTA_CLK_128 = not1(BYFE_CLK_128);
  /*_p15.KATY*/ wire KATY_APU_RSTn = not1(KEBA_APU_RSTp);
  /*#p15.JYNA*/ JYNA_CLK_64.dff17(HOTA_CLK_128, KATY_APU_RSTn, JYNA_CLK_64.qn_old());
  /*#p15.KYLO*/ wire KYLO_CLK_64 = not1(JYNA_CLK_64.qp_new());
  /*#p15.KENE*/ wire KENE_CLK_64 = not1(KYLO_CLK_64);

  //---------
  // Common address decoders

  /*_p07.BAKO*/ wire BAKO_ADDR_FFXXn = not1(SYKE_ADDR_FFXX);

  /*_p10.ATUP*/ wire ATUP_A04n = not1(BUS_CPU_A04p.out_new());
  /*_p10.BOXY*/ wire BOXY_A05n = not1(BUS_CPU_A05p.out_new());
  /*#p10.ASAD*/ wire ASAD_A06n = not1(BUS_CPU_A06p.out_new());
  /*#p10.AVUN*/ wire AVUN_A07n = not1(BUS_CPU_A07p.out_new());

  /*_p10.ATEG*/ wire ATEG_ADDR_XX1Xn = or4(BUS_CPU_A07p.out_new(), BUS_CPU_A06p.out_new(), BUS_CPU_A05p.out_new(), ATUP_A04n);
  /*_p10.AWET*/ wire AWET_ADDR_XX2Xn = or4(BUS_CPU_A07p.out_new(), BUS_CPU_A06p.out_new(), BOXY_A05n, BUS_CPU_A04p.out_new());

  /*_p10.BUNO*/ wire BUNO_ADDR_FF1Xp = nor2(BAKO_ADDR_FFXXn, ATEG_ADDR_XX1Xn);
  /*_p10.BANU*/ wire BANU_ADDR_FF1Xn = not1(BUNO_ADDR_FF1Xp);
  /*_p10.BEZY*/ wire BEZY_ADDR_FF2Xn = or2(AWET_ADDR_XX2Xn, BAKO_ADDR_FFXXn);
  /*_p10.CONA*/ wire CONA_ADDR_FF2Xp = not1(BEZY_ADDR_FF2Xn);

  /*_p10.DYTE*/ wire DYTE_ADDR_xxx0  = not1(BUS_CPU_A00p.out_new());
  /*_p10.AFOB*/ wire AFOB_ADDR_xx0x  = not1(BUS_CPU_A01p.out_new());
  /*_p10.ABUB*/ wire ABUB_ADDR_x0xx  = not1(BUS_CPU_A02p.out_new());
  /*_p10.ACOL*/ wire ACOL_ADDR_0xxx  = not1(BUS_CPU_A03p.out_new());

  /*_p10.DOSO*/ wire DOSO_ADDR_xxx1  = not1(DYTE_ADDR_xxx0);
  /*_p10.DUPA*/ wire DUPA_ADDR_xx1x  = not1(AFOB_ADDR_xx0x);
  /*_p10.DENO*/ wire DENO_ADDR_x1xx  = not1(ABUB_ADDR_x0xx);
  /*_p10.DUCE*/ wire DUCE_ADDR_1xxx  = not1(ACOL_ADDR_0xxx);

  //----------
  // FF24 NR50 write/read

  {
    /*_p10.DATU*/ wire DATU_ADDR_0100bn = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, AFOB_ADDR_xx0x, DYTE_ADDR_xxx0);
    /*_p10.CAFY*/ wire CAFY_ADDR_FF24   = nor2(BEZY_ADDR_FF2Xn, DATU_ADDR_0100bn);

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

    /*_p09.BYMA*/ wire BYMA_ADDR_FF24n  = not1(CAFY_ADDR_FF24);
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

    /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(AKOD_D00);
    /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(AWED_D01);
    /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(AVUD_D02);
    /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(AXEM_D03);
    /*_BUS_CPU_D04p*/ BUS_CPU_D04p.tri_bus(AMAD_D04);
    /*_BUS_CPU_D05p*/ BUS_CPU_D05p.tri_bus(ARUX_D05);
    /*_BUS_CPU_D06p*/ BUS_CPU_D06p.tri_bus(BOCY_D06);
    /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(ATUR_D07);
  }

  //----------
  // FF25 NR51

  {
    /*_p10.DURA*/ wire DURA_ADDR_0101n = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, AFOB_ADDR_xx0x, DOSO_ADDR_xxx1);
    /*_p10.CORA*/ wire CORA_ADDR_FF25p = nor2(BEZY_ADDR_FF2Xn, DURA_ADDR_0101n);

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

    /*_p09.GEPA*/ wire GEPA_ADDR_FF25n = not1(CORA_ADDR_FF25p);
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
  }

  //----------
  // FF26 NR52

  {
    /*_p10.EKAG*/ wire EKAG_ADDR_0110n = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
    /*_p10.DOXY*/ wire DOXY_ADDR_FF26p = and2(CONA_ADDR_FF2Xp,  EKAG_ADDR_0110n); // something not right here

    /*_p09.ETUC*/ wire ETUC_NR52_WRp = and2(DOXY_ADDR_FF26p, BOGY_CPU_WRp);
    /*_p09.FOKU*/ wire FOKU_NR52_WRn = not1(ETUC_NR52_WRp);
    /*_p09.BOPY*/ wire BOPY_NR52_WRn = nand2(DOXY_ADDR_FF26p, BOGY_CPU_WRp);
    /*_p09.HAWU*/ wire HAWU_NR52_WRn = nand2(DOXY_ADDR_FF26p, BOGY_CPU_WRp);

    /*_p09.EFOP*/ wire EFOP_NR52_DBG_APU_IN = and2(BUS_CPU_D04p.out_old(), UNOR_MODE_DBG2);
    /*_p09.FERO*/ FERO_NR52_DBG_APUn    .dff9 (FOKU_NR52_WRn, KEPY_APU_RSTn, EFOP_NR52_DBG_APU_IN);
    /*_p09.BOWY*/ BOWY_NR52_DBG_SWEEP   .dff17(BOPY_NR52_WRn, KEPY_APU_RSTn, BUS_CPU_D05p.out_old());
    /*_p09.HADA*/ HADA_NR52_ALL_SOUND_ON.dff17(HAWU_NR52_WRn, GUFO_SYS_RSTn, BUS_CPU_D07p.out_old()); // Since this bit controls APU_RESET*, it is reset by SYS_RESET.

    /*_p09.CETO*/ wire CETO_CPU_RDp = not1(AGUZ_CPU_RDn);
    /*_p09.KAZO*/ wire KAZO_CPU_RDp = not1(AGUZ_CPU_RDn);
    /*_p09.CURU*/ wire CURU_CPU_RDp = not1(AGUZ_CPU_RDn);
    /*_p09.KYDU*/ wire KYDU_CPU_RDp = not1(AGUZ_CPU_RDn);

    /*_p09.DOLE*/ wire DOLE_NR52_RDn = nand2(DOXY_ADDR_FF26p, CETO_CPU_RDp);
    /*_p09.DURU*/ wire DURU_NR52_RDn = nand2(DOXY_ADDR_FF26p, CURU_CPU_RDp);
    /*_p09.FEWA*/ wire FEWA_NR52_RDn = nand2(DOXY_ADDR_FF26p, GAXO_CPU_RDp);
    /*_p09.KAMU*/ wire KAMU_NR52_RDn = nand2(DOXY_ADDR_FF26p, KAZO_CPU_RDp);
    /*_p09.JURE*/ wire JURE_NR52_RDn = nand2(DOXY_ADDR_FF26p, KYDU_CPU_RDp);

    /*_p18.COKA*/ wire COKA_CH3_ACTIVEp = not1(DAVO_CH3_ACTIVEp.qn_new());
    /*_p13.CARA*/ wire CARA_CH1_ACTIVEn = not1(CYTO_CH1_ACTIVEp.qp_new());
    /*_p15.DEFU*/ wire DEFU_CH2_ACTIVEn = not1(DANE_CH2_ACTIVE.qp_new());
    /*_p18.ERED*/ wire ERED_CH3_ACTIVEn = not1(COKA_CH3_ACTIVEp);
    /*_p20.JUWA*/ wire JUWA_CH4_ACTIVEn = not1(GENA_CH4_ACTIVE.qp_new());

    /*_p09.COTO*/ triwire COTO_D0 = tri6_nn(DOLE_NR52_RDn, CARA_CH1_ACTIVEn);
    /*_p09.EFUS*/ triwire EFUS_D1 = tri6_nn(DURU_NR52_RDn, DEFU_CH2_ACTIVEn);
    /*_p09.FATE*/ triwire FATE_D2 = tri6_nn(FEWA_NR52_RDn, ERED_CH3_ACTIVEn);
    /*_p09.KOGE*/ triwire KOGE_D3 = tri6_nn(KAMU_NR52_RDn, JUWA_CH4_ACTIVEn);
    /*_BUS_CPU_D00p*/ BUS_CPU_D00p.tri_bus(COTO_D0);
    /*_BUS_CPU_D01p*/ BUS_CPU_D01p.tri_bus(EFUS_D1);
    /*_BUS_CPU_D02p*/ BUS_CPU_D02p.tri_bus(FATE_D2);
    /*_BUS_CPU_D03p*/ BUS_CPU_D03p.tri_bus(KOGE_D3);

    /*_p09.HOPE*/ triwire HOPE_D7 = tri6_nn(JURE_NR52_RDn, HADA_NR52_ALL_SOUND_ON.qn_new());
    /*_BUS_CPU_D07p*/ BUS_CPU_D07p.tri_bus(HOPE_D7);
  }













































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
    /*_p10.DUVU*/ wire DUVU_ADDR_0111an = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, DUPA_ADDR_xx1x, DOSO_ADDR_xxx1);
    /*_p10.DUTU*/ wire DUTU_ADDR_FF17  = nor2(BANU_ADDR_FF1Xn, DUVU_ADDR_0111an);

    // FIXME wtf with the different clock polarities? i guess it doesn't matter much.
    /*#p14.ENUF*/ wire ENUF = and2(DUTU_ADDR_FF17, BOGY_CPU_WRp);
    /*#p14.GERE*/ wire GERE = and2(BOGY_CPU_WRp, DUTU_ADDR_FF17);
    /*#p14.JEDE*/ wire JEDE = not1(GERE);

    /*_p14.HYFU*/ HYFU_NR22_P0.dff9(JEDE, JYBU_APU_RSTn, BUS_CPU_D00p.out_old());
    /*_p14.HORE*/ HORE_NR22_P1.dff9(JEDE, JYBU_APU_RSTn, BUS_CPU_D01p.out_old());
    /*_p14.HAVA*/ HAVA_NR22_P2.dff9(JEDE, JYBU_APU_RSTn, BUS_CPU_D02p.out_old());
    /*_p14.FORE*/ FORE_NR22_ADD.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D03p.out_old());
    /*_p14.GATA*/ GATA_NR22_V0.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D04p.out_old());
    /*_p14.GUFE*/ GUFE_NR22_V1.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D05p.out_old());
    /*_p14.GURA*/ GURA_NR22_V2.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D06p.out_old());
    /*_p14.GAGE*/ GAGE_NR22_V3.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D07p.out_old());

    /*_p14.GURE*/ wire GURE = not1(DUTU_ADDR_FF17);
    /*_p14.FYRY*/ wire FYRY = not1(DUTU_ADDR_FF17);
    /*_p14.GEXA*/ wire GEXA = or2(GURE, AGUZ_CPU_RDn);
    /*_p14.GURU*/ wire GURU = or2(FYRY, AGUZ_CPU_RDn);

    /*_p14.HUVU*/ triwire HUVU_D0 = tri6_nn(GEXA, HYFU_NR22_P0.qp_new());
    /*_p14.HYRE*/ triwire HYRE_D1 = tri6_nn(GEXA, HORE_NR22_P1.qp_new());
    /*_p14.HAVU*/ triwire HAVU_D2 = tri6_nn(GEXA, HYFU_NR22_P0.qp_new());
    /*_p14.GENE*/ triwire GENE_D3 = tri6_nn(GURU, FORE_NR22_ADD.qp_new());
    /*_p14.HUPE*/ triwire HUPE_D4 = tri6_nn(GURU, GATA_NR22_V0.qp_new());
    /*_p14.HERE*/ triwire HERE_D5 = tri6_nn(GURU, GUFE_NR22_V1.qp_new());
    /*_p14.HORO*/ triwire HORO_D6 = tri6_nn(GURU, GURA_NR22_V2.qp_new());
    /*_p14.HYRY*/ triwire HYRY_D7 = tri6_nn(GURU, GAGE_NR22_V3.qp_new());

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
    /*_p10.DAFY*/ wire DAFY_ADDR_1000an = nand4(DUCE_ADDR_1xxx, ABUB_ADDR_x0xx, AFOB_ADDR_xx0x, DYTE_ADDR_xxx0);
    /*_p10.DARA*/ wire DARA_ADDR_FF18  = nor2(BANU_ADDR_FF1Xn, DAFY_ADDR_1000an);
    /*_p14.DOSA*/ wire DOSA_NR23_WR1 = and2(DARA_ADDR_FF18, BOGY_CPU_WRp);
    /*_p14.EXUC*/ wire EXUC_NR23_WR2 = and2(DARA_ADDR_FF18, BOGY_CPU_WRp);
    /*_p14.ESUR*/ wire ESUR_NR23_WRn1 = not1(DOSA_NR23_WR1);
    /*_p14.FYXO*/ wire FYXO_NR23_WRn2 = not1(EXUC_NR23_WR2);
    /*_p14.HUDE*/ wire HUDE_APU_RESETn1 = not1(KEBA_APU_RSTp);
    /*_p14.FOFE*/ FOFE_NR23_FREQ_00.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D00p.out_old());
    /*_p14.FOVA*/ FOVA_NR23_FREQ_01.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D01p.out_old());
    /*_p14.FEDY*/ FEDY_NR23_FREQ_02.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D02p.out_old());
    /*_p14.FOME*/ FOME_NR23_FREQ_03.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D03p.out_old());
    /*_p14.FORA*/ FORA_NR23_FREQ_04.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D04p.out_old());
    /*_p14.GODA*/ GODA_NR23_FREQ_05.dff9(FYXO_NR23_WRn2, HUDE_APU_RESETn1, BUS_CPU_D05p.out_old());
    /*_p14.GUMY*/ GUMY_NR23_FREQ_06.dff9(FYXO_NR23_WRn2, HUDE_APU_RESETn1, BUS_CPU_D06p.out_old());
    /*_p14.GUPU*/ GUPU_NR23_FREQ_07.dff9(FYXO_NR23_WRn2, HUDE_APU_RESETn1, BUS_CPU_D07p.out_old());

    /*_p14.FERY*/ wire FERY = not1(DUCE_ADDR_1xxx);
    /*_p14.GUZA*/ wire GUZA = nor2(FERY, FAPE_DBG_CPU_RDp);
    /*_p14.FUTY*/ wire FUTY = not1(GUZA);
    /*#p14.FAVA*/ triwire FAVA_D0 = tri6_nn(FUTY, DONE_CH2_FREQ_00.qn_new());
    /*#p14.FAJY*/ triwire FAJY_D1 = tri6_nn(FUTY, DYNU_CH2_FREQ_01.qn_new());
    /*#p14.FEGU*/ triwire FEGU_D2 = tri6_nn(FUTY, EZOF_CH2_FREQ_02.qn_new());
    /*#p14.FOSE*/ triwire FOSE_D3 = tri6_nn(FUTY, CYVO_CH2_FREQ_03.qn_new());
    /*#p14.GERO*/ triwire GERO_D4 = tri6_nn(FUTY, FUXO_CH2_FREQ_04.qn_new()); // d4 and d5 are switched on the schematic
    /*#p14.GAKY*/ triwire GAKY_D5 = tri6_nn(FUTY, GANO_CH2_FREQ_05.qn_new());
    /*#p14.GADU*/ triwire GADU_D6 = tri6_nn(FUTY, GOCA_CH2_FREQ_06.qn_new());
    /*#p14.GAZO*/ triwire GAZO_D7 = tri6_nn(FUTY, GANE_CH2_FREQ_07.qn_new());

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


  {
    /*_p10.DEJY*/ wire DEJY_ADDR_1001an = nand4(DUCE_ADDR_1xxx, ABUB_ADDR_x0xx, AFOB_ADDR_xx0x, DOSO_ADDR_xxx1);
    /*_p10.DOZA*/ wire DOZA_ADDR_FF19  = nor2(BANU_ADDR_FF1Xn, DEJY_ADDR_1001an);

    /*_pXX.JENU*/ wire JENU_NR24_WRp = and2(DOZA_ADDR_FF19, BOGY_CPU_WRp);
    /*_pXX.KYSA*/ wire KYSA_NR24_WRn = not1(JENU_NR24_WRp);
    /*_p14.KYPU*/ wire KYPU_APU_RSTn = not1(KEBA_APU_RSTp);
    /*_pXX.JEFU*/ JEFU_NR24_FREQ_08.dff9(KYSA_NR24_WRn, KYPU_APU_RSTn, BUS_CPU_D00p.out_old());
    /*_pXX.JANY*/ JANY_NR24_FREQ_09.dff9(KYSA_NR24_WRn, KYPU_APU_RSTn, BUS_CPU_D01p.out_old());
    /*_pXX.JUPY*/ JUPY_NR24_FREQ_10.dff9(KYSA_NR24_WRn, KYPU_APU_RSTn, BUS_CPU_D02p.out_old());

    /*_pXX.EVYF*/ wire EVYF = nand2(ANUJ_CPU_WR_WEIRD, DOZA_ADDR_FF19);
    /*_p14.FAZO*/ wire FAZO_APU_RSTn = not1(KEBA_APU_RSTp);
    /*_pXX.EMER*/ EMER_NR24_LEN.dff9(EVYF, FAZO_APU_RSTn, BUS_CPU_D06p.out_old());

    /*#pXX.GADO*/ wire GADO = not1(AGUZ_CPU_RDn);
    /*#pXX.HUMA*/ wire HUMA = nand2(DOZA_ADDR_FF19, GADO); // why was this nor2? doublecheck
    /*#pXX.GOJY*/ triwire GOJY_D6 = tri6_nn(HUMA, EMER_NR24_LEN.qp_new());
    BUS_CPU_D06p.tri_bus(GOJY_D6);

    // This resets ETAP 8 phases after it's set?
    /*_p15.CYWU*/ wire CYWU_APU_RSTn = not1(KEBA_APU_RSTp);
    /*#p15.DOPE*/ DOPE_CH2_TRIG.dff17(DOVA_ABCDxxxx, CYWU_APU_RSTn, ETAP_NR24_TRIG.qn_old());

    /*#p??.DETA*/ wire DETA_NR24_WRn = nand2(BOGY_CPU_WRp, DOZA_ADDR_FF19);
    /*#p15.DERA*/ wire DERA_RSTn = nor2(KEBA_APU_RSTp, DOPE_CH2_TRIG.qp_new());
    /*#p??.ETAP*/ ETAP_NR24_TRIG.dff9(DETA_NR24_WRn, DERA_RSTn, BUS_CPU_D07p.out_old());

    /*#p14.GOTE*/ wire GOTE = not1(DOZA_ADDR_FF19);
    /*#p14.HYPO*/ wire HYPO = or2(GOTE, FAPE_DBG_CPU_RDp);
    /*#p14.HUNA*/ triwire HUNA_D0 = tri6_nn(HYPO, HEVY_CH2_FREQ_08.qn_new());
    /*#p14.JARO*/ triwire JARO_D1 = tri6_nn(HYPO, HEPU_CH2_FREQ_09.qn_new());
    /*#p14.JEKE*/ triwire JEKE_D2 = tri6_nn(HYPO, HERO_CH2_FREQ_10.qn_new());

    BUS_CPU_D00p.tri_bus(HUNA_D0);
    BUS_CPU_D01p.tri_bus(JARO_D1);
    BUS_CPU_D02p.tri_bus(JEKE_D2);
  }

  /*_p15.DOXA*/ wire DOXA = or2(KEBA_APU_RSTp, DORY.qp_new());
  /*_p15.CELO*/ wire CELO = not1(DOXA);
  /*_p15.DALA*/ wire DALA = or2(CELO, DOPE_CH2_TRIG.qp_new());
  /*_p15.ELOX*/ ELOX.dff17(CEMO_1MHZ.qp_new(), DOXA,          DALA);
  /*_p15.DORY*/ DORY.dff17(CEMO_1MHZ.qp_new(), CEXE_APU_RSTn, ELOX.qp_new());
  /*_p15.CAZA*/ CAZA.dff17(CEMO_1MHZ.qp_new(), CEXE_APU_RSTn, DORY.qp_old());


  /*#p15.FUTE*/ wire FUTE_CH2_AMP_ENn = nor5(FORE_NR22_ADD.qn_new(), GATA_NR22_V0.qn_new(), GUFE_NR22_V1.qn_new(), GURA_NR22_V2.qn_new(), GAGE_NR22_V3.qn_new());

  // this is freq overflow or something
  /*#p15.FUJY*/ wire FUJY = and2(GYKO_CLK.qp_new(), CEMO_1MHZ.qp_new());
  /*#p15.GALU*/ wire GALU = not1(HERO_CH2_FREQ_10.qp_new());
  /*#p15.GYRE*/ wire GYRE = nor3(KEBA_APU_RSTp, ELOX.qp_new(), FUJY);
  /*#p15.GYKO*/ GYKO_CLK.dff17(GALU, GYRE, GYKO_CLK.qn_old());
  /*#p15.ETUK*/ wire ETUK_CLK = not1(GYKO_CLK.qp_new());
  /*#p15.DAVU*/ wire DAVU_CLK = not1(ETUK_CLK);


  {
    /*#p15.CULE*/ wire CULE_CLK = not1(DAVU_CLK);
    /*#p15.CANO*/ CANO_00.dff17(CULE_CLK,         AFAT_APU_RSTn, CANO_00.qn_old());
    /*#p15.CAGY*/ CAGY_01.dff13(CANO_00.qn_new(), AFAT_APU_RSTn, CAGY_01.qn_old());
    /*#p15.DYVE*/ DYVE_02.dff13(CAGY_01.qn_new(), AFAT_APU_RSTn, DYVE_02.qn_old());
  }

  {
    /*#p15.ARES*/ wire ARES = nor2(FUTE_CH2_AMP_ENn, KEBA_APU_RSTp); // looks like nor on the die?
    /*#p15.BODO*/ wire BODO = not1(CAZA.qp_new());
    /*_p15.BUTA*/ BUTA_FREQ_GATEp.nand_latch(ARES, BODO);
    /*#p15.CAMA*/ wire CAMA_FREQ_TICKp = nor2(CEMO_1MHZ.qp_new(), BUTA_FREQ_GATEp.qp_new()); // no idea of buta polarity
    /*_p15.DOCA*/ wire DOCA_FREQ_TICKn = not1(CAMA_FREQ_TICKp);
    /*#p15.DUJU*/ wire DUJU_LOADn = nor2(DAVU_CLK, ELOX.qp_new());

    /*#p15.COGU*/ wire COGU_LOADp = not1(DUJU_LOADn);
    /*_p14.DONE*/ DONE_CH2_FREQ_00.dff20(DOCA_FREQ_TICKn,            COGU_LOADp, FOFE_NR23_FREQ_00.qp_new());
    /*_p14.DYNU*/ DYNU_CH2_FREQ_01.dff20(DONE_CH2_FREQ_00.qp_new(),  COGU_LOADp, FOVA_NR23_FREQ_01.qp_new());
    /*_p14.EZOF*/ EZOF_CH2_FREQ_02.dff20(DYNU_CH2_FREQ_01.qp_new(),  COGU_LOADp, FEDY_NR23_FREQ_02.qp_new());
    /*_p14.CYVO*/ CYVO_CH2_FREQ_03.dff20(EZOF_CH2_FREQ_02.qp_new(),  COGU_LOADp, FOME_NR23_FREQ_03.qp_new());

    /*#p14.EDEP*/ wire EDEP_CH2_FREQ_03 = not1(CYVO_CH2_FREQ_03.qn_new());
    /*#p15.EROG*/ wire EROG_LOADp = not1(DUJU_LOADn);
    /*_p14.FUXO*/ FUXO_CH2_FREQ_04.dff20(EDEP_CH2_FREQ_03,           EROG_LOADp, FORA_NR23_FREQ_04.qp_new());
    /*_p14.GANO*/ GANO_CH2_FREQ_05.dff20(FUXO_CH2_FREQ_04.qp_new(),  EROG_LOADp, GODA_NR23_FREQ_05.qp_new());
    /*_p14.GOCA*/ GOCA_CH2_FREQ_06.dff20(GANO_CH2_FREQ_05.qp_new(),  EROG_LOADp, GUMY_NR23_FREQ_06.qp_new());
    /*_p14.GANE*/ GANE_CH2_FREQ_07.dff20(GOCA_CH2_FREQ_06.qp_new(),  EROG_LOADp, GUPU_NR23_FREQ_07.qp_new());

    /*#p14.GALA*/ wire GALA_CH2_FREQ_07 = not1(GANE_CH2_FREQ_07.qn_new());
    /*#p15.GYPA*/ wire GYPA_LOADp = not1(DUJU_LOADn);
    /*_p14.HEVY*/ HEVY_CH2_FREQ_08.dff20(GALA_CH2_FREQ_07,           GYPA_LOADp, JEFU_NR24_FREQ_08.qp_new());
    /*_p14.HEPU*/ HEPU_CH2_FREQ_09.dff20(HEVY_CH2_FREQ_08.qp_new(),  GYPA_LOADp, JANY_NR24_FREQ_09.qp_new());
    /*_p14.HERO*/ HERO_CH2_FREQ_10.dff20(HEPU_CH2_FREQ_09.qp_new(),  GYPA_LOADp, JUPY_NR24_FREQ_10.qp_new());
  }


  //----------


  /*_p15.HYLY*/ wire HYLY = nor2(ELOX.qp_new(), JOPA.qp_new());
  /*_p15.JAKE*/ wire JAKE = not1(HYLY);
  
  /*#p15.JORE*/ JORE.dff20(KENE_CLK_64,   JAKE, HYFU_NR22_P0.qp_new());
  /*#p15.JONA*/ JONA.dff20(JORE.qp_new(), JAKE, HORE_NR22_P1.qp_new());
  /*#p15.JEVY*/ JEVY.dff20(JONA.qp_new(), JAKE, HAVA_NR22_P2.qp_new());

  /*#p15.JUPU*/ wire JUPU = nor3(HYFU_NR22_P0.qn_new(), HORE_NR22_P1.qn_new(), HAVA_NR22_P2.qn_new());

  
  {
    /*#p15.GADE*/ wire GADE = not1(JOPA.qp_new());
    /*#p15.HOLY*/ wire HOLY = nor2(HORU_CLK_512, GADE);
    /*#p15.HAFE*/ wire HAFE = nor4(HOLY, JUPU, ELOX.qp_new(), KEBA_APU_RSTp); // schematic wrong this is nor4
    /*#p15.KYVO*/ wire KYVO = and3(JEVY.qp_new(), JONA.qp_new(), JORE.qp_new());
    /*#p15.JOPA*/ JOPA.dff17(HORU_CLK_512, HAFE, KYVO);
  }


  // something envelope related?
  {
    /*_p15.HYPA*/ wire HYPA = nor2(ELOX.qp_new(), KEBA_APU_RSTp);
    /*#p15.EMYR*/ wire EMYR = nor5 (FORE_NR22_ADD.qn_new(), FENO.qp_new(), FETE.qp_new(), FOMY.qp_new(), FENA.qp_new());
    /*#p15.ERAT*/ wire ERAT = nand5(FORE_NR22_ADD.qn_new(), FENO.qp_new(), FETE.qp_new(), FOMY.qp_new(), FENA.qp_new());
    /*#p15.FYRE*/ wire FYRE = not1(ERAT);
    /*#p15.GUFY*/ wire GUFY = or2(EMYR, FYRE);
    /*#p15.HEPO*/ HEPO.dff17(JOPA.qp_new(), HYPA, GUFY);
    /*_p15.HYLE*/ wire HYLE = or2(KEBA_APU_RSTp, ELOX.qp_new());
    /*#p15.JEME*/ JEME.nor_latch(HEPO.qp_new(), HYLE);
  }
  


  /*_p15.DORA*/ wire DORA = and2(CYRE.qp_new(), EMER_NR24_LEN.qn_new());
  /*_p15.ESYK*/ wire ESYK = or3(KEBA_APU_RSTp, DORA, FUTE_CH2_AMP_ENn);
  /*_p15.DANE*/ DANE_CH2_ACTIVE.nor_latch(ELOX.qp_new(), ESYK);






  {
    // NR21 write
    /*_p15.DEME*/ wire DEME = nor3(CYRE.qp_new(), BUFY_CLK_256, EMER_NR24_LEN.qp_new()); // why was this or2?
    /*_p15.DYRO*/ wire DYRO_CH2_LEN_CLK = not1(DEME);

    /*_p10.DAZA*/ wire DAZA_ADDR_0110n = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
    /*_p10.COVY*/ wire COVY_ADDR_FF16p   = nor2(BANU_ADDR_FF1Xn, DAZA_ADDR_0110n);
    /*_p14.AGYN*/ wire AGYN_FF16_WRn = nand2(BOGY_CPU_WRp, COVY_ADDR_FF16p);
    /*_p15.BYMO*/ wire BYMO_FF16_WRp = not1(AGYN_FF16_WRn);
    /*_p15.AGET*/ wire AGET_FF16_WRp = not1(AGYN_FF16_WRn);

    /*_p15.ERYC*/ ERYC_NR21_L0.dff20(DYRO_CH2_LEN_CLK,      BYMO_FF16_WRp, BUS_CPU_D00p.out_old());
    /*_p15.CERA*/ CERA_NR21_L1.dff20(ERYC_NR21_L0.qp_new(), BYMO_FF16_WRp, BUS_CPU_D01p.out_old());
    /*_p15.CONU*/ CONU_NR21_L2.dff20(CERA_NR21_L1.qp_new(), BYMO_FF16_WRp, BUS_CPU_D02p.out_old());
    /*_p15.CAME*/ CAME_NR21_L3.dff20(CONU_NR21_L2.qp_new(), BYMO_FF16_WRp, BUS_CPU_D03p.out_old());

    /*#p15.BUKO*/ wire BUKO_CH2_LEN_CLK = not1(CAME_NR21_L3.qn_new());
    /*_p15.BUVA*/ BUVA_NR21_L4.dff20(BUKO_CH2_LEN_CLK,      AGET_FF16_WRp, BUS_CPU_D04p.out_old());
    /*_p15.AKYD*/ AKYD_NR21_L5.dff20(BUVA_NR21_L4.qp_new(), AGET_FF16_WRp, BUS_CPU_D05p.out_old());

    /*_p14.BACU*/ wire BACU_FF16_WRp = and2(COVY_ADDR_FF16p, BOGY_CPU_WRp);
    /*_p14.BUDU*/ wire BUDU_FF16_WRn = not1(BACU_FF16_WRp);
    /*_p14.BERA*/ BERA_NR21_D6.dff9(BUDU_FF16_WRn, AFAT_APU_RSTn, BUS_CPU_D06p.out_old());
    /*_p14.BAMY*/ BAMY_NR21_D7.dff9(BUDU_FF16_WRn, AFAT_APU_RSTn, BUS_CPU_D07p.out_old());

    // CH2 length expire flag?
    /*_p14.ASYP*/ wire ASYP_FF16_WRp = not1(AGYN_FF16_WRn);
    /*_p14.BENY*/ wire BENY = nor3(ASYP_FF16_WRp, KEBA_APU_RSTp, ELOX.qp_new());
    /*_p15.CYRE*/ CYRE.dff17(AKYD_NR21_L5.qn_new(), BENY, CYRE.qn_old());

    // NR21 read
    /*_p14.BYGO*/ wire BYGO = not1(AGUZ_CPU_RDn);
    /*_p14.CORO*/ wire CORO = nand2(COVY_ADDR_FF16p, BYGO);

    /*_p14.CECY*/ triwire CECY_D6 = tri6_nn(CORO, BERA_NR21_D6.qp_new());
    /*_p14.CEKA*/ triwire CEKA_D7 = tri6_nn(CORO, BAMY_NR21_D7.qp_new());
    
    BUS_CPU_D06p.tri_bus(CECY_D6);
    BUS_CPU_D07p.tri_bus(CEKA_D7);
  }


  {
    // CH2 volume? envelope?
    /*_p15.HOFO*/ wire HOFO = or3(JOPA.qp_new(), JUPU, JEME.qp_new());
    /*#p15.GAFA*/ wire GAFA = amux2(FORE_NR22_ADD.qn_new(), HOFO,          HOFO,          FORE_NR22_ADD.qp_new());
    /*#p15.FARU*/ wire FARU = amux2(FORE_NR22_ADD.qn_new(), FENO.qp_new(), FENO.qn_new(), FORE_NR22_ADD.qp_new());
    /*#p15.ETUP*/ wire ETUP = amux2(FORE_NR22_ADD.qn_new(), FETE.qp_new(), FETE.qn_new(), FORE_NR22_ADD.qp_new());
    /*#p15.FOPY*/ wire FOPY = amux2(FORE_NR22_ADD.qn_new(), FOMY.qp_new(), FOMY.qn_new(), FORE_NR22_ADD.qp_new());

    /*#p15.FENO*/ FENO.dff20(GAFA, ELOX.qp_new(), FENO.qn_old());
    /*#p15.FETE*/ FETE.dff20(FARU, ELOX.qp_new(), GUFE_NR22_V1.qn_old());
    /*#p15.FOMY*/ FOMY.dff20(ETUP, ELOX.qp_new(), GURA_NR22_V2.qn_old());
    /*#p15.FENA*/ FENA.dff20(FOPY, ELOX.qp_new(), GAGE_NR22_V3.qn_old());
  }

  {
    // CH2 duty cycle generator
    /*#p15.DOJU*/ wire DOJU = nor2(BERA_NR21_D6.qn_new(), BAMY_NR21_D7.qp_new());
    /*#p15.DYTA*/ wire DYTA = nor2(BERA_NR21_D6.qp_new(), BAMY_NR21_D7.qn_new());
    /*#p15.DOMO*/ wire DOMO = nor2(BERA_NR21_D6.qn_new(), BAMY_NR21_D7.qn_new());
    /*#p15.DOVE*/ wire DOVE = nor2(BERA_NR21_D6.qp_new(), BAMY_NR21_D7.qp_new());
    /*#p15.DUGE*/ wire DUGE = not1(CANO_00.qp_new());
    /*#p15.DYMU*/ wire DYMU = and2(DYVE_02.qp_new(), CAGY_01.qp_new());
    /*#p15.EGOG*/ wire EGOG = and2(DYMU, DUGE);
    /*#p15.DARE*/ wire DARE = not1(DYMU);
    /*#p15.EXES*/ wire EXES_WAVE = amux4(EGOG, DOMO, DYMU, DYTA, DYVE_02.qp_new(), DOJU, DARE, DOVE);
    /*#p15.DOME*/ DOME_CH2_OUT.dff17(DAVU_CLK, AFAT_APU_RSTn, EXES_WAVE);
  }

  {
    // CH2 output to DAC
    // dac 2 inputs ANAN ANYV ASOG AMOV
    /*#p15.CYSE*/ wire CYSE = and2(DANE_CH2_ACTIVE.qp_new(), DOME_CH2_OUT.qp_new());
    /*#p15.BONU*/ wire BONU = or2(CYSE, EDEK_NR52_DBG_APUn);
    /*#p15.ANYV*/ wire ANYV = and2(FETE.qp_new(), BONU);
    /*#p15.ANAN*/ wire ANAN = and2(FENO.qp_new(), BONU);
    /*#p15.AMOV*/ wire AMOV = and2(FENA.qp_new(), BONU);
    /*#p15.ASOG*/ wire ASOG = and2(FOMY.qp_new(), BONU);
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

  /*_p10.EXAT*/ wire EXAT_ADDR_1010an = nand4(DUCE_ADDR_1xxx, ABUB_ADDR_x0xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
  /*_p10.EMOR*/ wire EMOR_ADDR_FF1A  = nor2(BANU_ADDR_FF1Xn, EXAT_ADDR_1010an);

  /*_p16.GOVE*/ wire GOVE_APU_RESETo = not1(KEBA_APU_RSTp);
  /*_p16.GEJO*/ wire GEJO_FF1A_WR = and2(BOGY_CPU_WRp, EMOR_ADDR_FF1A);
  /*_p16.GUCY*/ wire GUCY_FF1A_WRn = not1(GEJO_FF1A_WR);
  /*_p16.GUXE*/ GUXE_CH3_AMP_ENna.dff9(GUCY_FF1A_WRn, GOVE_APU_RESETo, BUS_CPU_D00p.out_old());

  /*_p16.FASY*/ wire FASY_FF1A_RD = nand2(EMOR_ADDR_FF1A, GAXO_CPU_RDp);
  /*_p16.FEVO*/ wire FEVO_CH3_AMP_ENa  = not1(GUXE_CH3_AMP_ENna.qn_new());

#if 0
  if (FASY_FF1A_RD) set_data(
    /*_p16.GEKO*/ FEVO_CH3_AMP_ENa
  );
#endif

  //----------
  // FF1B - NR31 - Channel 3 Sound Length

  /*_p10.EMAX*/ wire EMAX_ADDR_1011an = nand4(DOSO_ADDR_xxx1, DUPA_ADDR_xx1x, ABUB_ADDR_x0xx, DUCE_ADDR_1xxx);
  /*_p10.DUSA*/ wire DUSA_ADDR_FF1B  = nor2(BANU_ADDR_FF1Xn, EMAX_ADDR_1011an);
  /*_p16.DERY*/ wire DERY_FF1B_WR = nand2(BOGY_CPU_WRp, DUSA_ADDR_FF1B);

  /*#p18.GEPY*/ wire GEPY = nor3(FEXU_LEN_DONE.qp_new(), BUFY_CLK_256, HOTO_NR34_STOP.qp_new());

  /*#p18.GENU*/ wire GENU_CLK = not1(GEPY);
  /*_p16.GAJY*/ wire GAJY = not1(DERY_FF1B_WR);
  /*_p18.GEVO*/ GEVO_CH3_LEN0.dff20(GENU_CLK,               GAJY, BUS_CPU_D00p.out_old());
  /*_p18.FORY*/ FORY_CH3_LEN1.dff20(GEVO_CH3_LEN0.qp_new(), GAJY, BUS_CPU_D01p.out_old());
  /*_p18.GATU*/ GATU_CH3_LEN2.dff20(FORY_CH3_LEN1.qp_new(), GAJY, BUS_CPU_D02p.out_old());
  /*_p18.GAPO*/ GAPO_CH3_LEN3.dff20(GATU_CH3_LEN2.qp_new(), GAJY, BUS_CPU_D03p.out_old());

  /*#p18.FALU*/ wire FALU_CLK = not1(GAPO_CH3_LEN3.qn_new());
  /*_p16.EMUT*/ wire EMUT = not1(DERY_FF1B_WR);
  /*_p18.GEMO*/ GEMO_CH3_LEN4.dff20(FALU_CLK,               EMUT, BUS_CPU_D04p.out_old());
  /*_p18.FORO*/ FORO_CH3_LEN5.dff20(GEMO_CH3_LEN4.qp_new(), EMUT, BUS_CPU_D05p.out_old());
  /*_p18.FAVE*/ FAVE_CH3_LEN6.dff20(FORO_CH3_LEN5.qp_new(), EMUT, BUS_CPU_D06p.out_old());
  /*_p18.FYRU*/ FYRU_CH3_LEN7.dff20(FAVE_CH3_LEN6.qp_new(), EMUT, BUS_CPU_D07p.out_old());

  /*#p16.GETO*/ wire GETO = not1(DERY_FF1B_WR);
  /*#p18.GUDA*/ wire GUDA = nor3(GETO, KEBA_APU_RSTp, GARA.qp_new());
  /*_p18.FEXU*/ FEXU_LEN_DONE.dff17(FYRU_CH3_LEN7.qn_new(), GUDA, FEXU_LEN_DONE.qn_old());

  //----------
  // FF1C NR32 - Channel 3 Select output level (R/W)

  /*_p10.GANY*/ wire GANY_ADDR_1100an = nand4(DUCE_ADDR_1xxx, DENO_ADDR_x1xx, AFOB_ADDR_xx0x, DYTE_ADDR_xxx0);
  /*_p10.GEFO*/ wire GEFO_ADDR_FF1C  = nor2(BANU_ADDR_FF1Xn, GANY_ADDR_1100an);

  /*_p16.GURO*/ wire GURO_APU_RESETn = not1(KEBA_APU_RSTp);

  /*_p16.HAGA*/ wire HAGA_FF1C_WR = and2(BOGY_CPU_WRp, GEFO_ADDR_FF1C);

  /*_p16.GUZU*/ wire GUZU_FF1C_WRn = not1(HAGA_FF1C_WR);
  /*_p16.HUKY*/ HUKY_NR32_VOL0.dff9(GUZU_FF1C_WRn, GURO_APU_RESETn, BUS_CPU_D00p.out_old());
  /*_p16.HODY*/ HODY_NR32_VOL1.dff9(GUZU_FF1C_WRn, GURO_APU_RESETn, BUS_CPU_D01p.out_old());

  /*_p16.JOTU*/ wire JOTU_CPU_RDb = not1(AGUZ_CPU_RDn);
  /*_p16.HENU*/ wire HENU_FF1C_RD = nand2(GEFO_ADDR_FF1C, JOTU_CPU_RDb);

  /*_p18.JECO*/ wire JECO_DBG_CPU_RDb = not1(AGUZ_CPU_RDn);
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

  //----------
  // FF1D - NR33 - Channel 3 Frequency's lower data (W)

  /*_p10.EMOS*/ wire EMOS_ADDR_1101an = nand4(DOSO_ADDR_xxx1, AFOB_ADDR_xx0x, DENO_ADDR_x1xx, DUCE_ADDR_1xxx);
  /*_p10.FENY*/ wire FENY_ADDR_FF1D  = nor2(BANU_ADDR_FF1Xn, EMOS_ADDR_1101an);
  /*_p16.HOXA*/ wire HOXA_ADDR_FF1Dn = not1(FENY_ADDR_FF1D);

  /*_p16.KOTA*/ wire KOTA_FF1D_WRn = nand2(FENY_ADDR_FF1D, BOGY_CPU_WRp);
  /*_p16.JAFA*/ wire JAFA_FF1D_WRo = nand2(FENY_ADDR_FF1D, BOGY_CPU_WRp);

  /*_p16.KYHO*/ wire KYHO_FF1D_WRa = not1(KOTA_FF1D_WRn);
  /*_p16.KULY*/ wire KULY_FF1D_WRb = not1(JAFA_FF1D_WRo);
  /*_p16.KUHA*/ wire KUHA_APU_RESETq = not1(KEBA_APU_RSTp);

  /*_p16.KOGA*/ KOGA_NR33_FREQ00.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D00p.out_old());
  /*_p16.JOVY*/ JOVY_NR33_FREQ01.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D01p.out_old());
  /*_p16.JAXA*/ JAXA_NR33_FREQ02.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D02p.out_old());
  /*_p16.JEFE*/ JEFE_NR33_FREQ03.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D03p.out_old());
  /*_p16.JYPO*/ JYPO_NR33_FREQ04.dff9(KULY_FF1D_WRb, KUHA_APU_RESETq, BUS_CPU_D04p.out_old());
  /*_p16.JOVE*/ JOVE_NR33_FREQ05.dff9(KYHO_FF1D_WRa, KUHA_APU_RESETq, BUS_CPU_D05p.out_old());
  /*_p16.KANA*/ KANA_NR33_FREQ06.dff9(KYHO_FF1D_WRa, KUHA_APU_RESETq, BUS_CPU_D06p.out_old());
  /*_p16.KOGU*/ KOGU_NR33_FREQ07.dff9(KYHO_FF1D_WRa, KUHA_APU_RESETq, BUS_CPU_D07p.out_old());

  /*_p16.DOVO*/ wire DOVO_CPU_RDp = not1(AGUZ_CPU_RDn);
  /*_p16.EGAD*/ wire EGAD_DBG_CPU_RD = nand2(DOVO_CPU_RDp, EDEK_NR52_DBG_APUn);
  /*_p16.GUTE*/ wire GUTE_DBG_FF1D_RDn = nor2(HOXA_ADDR_FF1Dn, EGAD_DBG_CPU_RD);
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

  //----------
  // FF1E - NR34 - Channel 3 Frequency's higher data (R/W)

  /*_p10.EGEN*/ wire EGEN_ADDR_1110an = nand4(DUCE_ADDR_1xxx, DENO_ADDR_x1xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
  /*_p10.DUGO*/ wire DUGO_ADDR_FF1E  = nor2(BANU_ADDR_FF1Xn, EGEN_ADDR_1110an);

  /*_p16.HUDA*/ wire HUDA_FF1E_WRn = nand2(DUGO_ADDR_FF1E, BOGY_CPU_WRp); // polarity?

  /*_p16.KOPY*/ wire KOPY_APU_RESETs  = not1(KEBA_APU_RSTp);
  /*_p16.HEKY*/ wire HEKY_APU_RESETr  = not1(KEBA_APU_RSTp);
  /*_p16.FAKO*/ wire FAKO_RESTART_RST = nor2(KEBA_APU_RSTp, FOBA_CH3_RESTART_SYNC.qp_new());

  /*_p16.JUZO*/ wire JUZO_FF1E_WR = not1(HUDA_FF1E_WRn);
  /*_p16.FOVO*/ wire FOVO_FF1E_WRo = nand2(ANUJ_CPU_WR_WEIRD, DUGO_ADDR_FF1E);
  /*_p16.EPYX*/ wire EPYX_FF1E_WRp = nor2(BOGY_CPU_WRp, DUGO_ADDR_FF1E); // polarity?

  /*_p16.JEMO*/ JEMO_NR34_FREQ08.dff9 (JUZO_FF1E_WR,  KOPY_APU_RESETs,  BUS_CPU_D00p.out_old());
  /*_p16.JETY*/ JETY_NR34_FREQ09.dff9 (JUZO_FF1E_WR,  KOPY_APU_RESETs,  BUS_CPU_D01p.out_old());
  /*_p16.JACY*/ JACY_NR34_FREQ10.dff9(JUZO_FF1E_WR,  KOPY_APU_RESETs,  BUS_CPU_D02p.out_old());
  /*_p16.HOTO*/ HOTO_NR34_STOP.dff9  (FOVO_FF1E_WRo, HEKY_APU_RESETr,  BUS_CPU_D03p.out_old());
  /*_p16.GAVU*/ GAVU_NR34_START.dff9 (EPYX_FF1E_WRp, FAKO_RESTART_RST, BUS_CPU_D04p.out_old());


  /*_p16.GUNU*/ wire GUNU = not1(DUGO_ADDR_FF1E);
  /*_p16.FUVA*/ wire FUVA_FF14_RDb = or2(GUNU, EGAD_DBG_CPU_RD);

#if 0
  if (FUVA_FF14_RDb) set_data(
    /*_p16.JUVY*/ KEJU_FREQ_08,
    /*_p16.JURA*/ KEZA_FREQ_09,
    /*_p16.HUFO*/ JAPU_FREQ_10,
    /*_p16.HACA*/ HOTO_NR34_STOP
  );
#endif

  /*_p16.GORY*/ wire GORY = not1(AGUZ_CPU_RDn); // polarity?
  /*_p16.GAWA*/ wire GAWA_FF14_RDa = nand2(DUGO_ADDR_FF1E, GORY); // polarity?

#if 0
  if (GAWA_FF14_RDa) set_data(
    /*_p16.HACA*/ HOTO_NR34_STOP
  );
#endif

  //----------
  // Frequency counter

  /*#p16.FURY*/ wire FURY_APU_RESETn = nor2(KEBA_APU_RSTp, GYTA.qp_new());
  /*#p16.GULO*/ wire GULO = not1(FURY_APU_RESETn);

  // weird latch?

  /*_p16.GOFY*/ GOFY.nor_latch(GULO, FOBA_CH3_RESTART_SYNC.qp_new());

  /*_p16.GAZE*/ wire GAZE_APU_RESETo = not1(KEBA_APU_RSTp);

  /*#p16.FABO*/ wire FABO_CLK_xxCDxxGH = not1(spu.CERY_2M.qp_new());
  
  // shift reg delay?
  /*#p16.GARA*/ GARA.dff17(FABO_CLK_xxCDxxGH, FURY_APU_RESETn, GOFY.qn_new());
  /*#p16.GYTA*/ GYTA.dff17(FABO_CLK_xxCDxxGH, GAZE_APU_RESETo, GARA.qp_new());
  /*#p16.GYRA*/ GYRA.dff17(spu.CERY_2M.qp_new(),  GAZE_APU_RESETo, GYTA.qp_new());

  /*#p16.FAJU*/ wire FAJU = not1(GYRA.qp_new());
  /*_p16.FUVO*/ wire FUVO = nor2(GUXE_CH3_AMP_ENna.qp_new(), KEBA_APU_RSTp);

  /*#p16.GUGU*/ GUGU_FREQ_CLK_STOP.nand_latch(FUVO, FAJU);

  /*#p18.HEFO*/ wire HEFO_FREQ_CLKn = nor2(spu.CERY_2M.qp_new(), GUGU_FREQ_CLK_STOP.qp_new());

  /*#p18.HEMA*/ wire HEMA_WAVE_CLKb = not1(HUNO_WAVE_CLKo.qp_new());
  /*#p18.GASE*/ wire GASE_WAVE_CLKn = not1(HEMA_WAVE_CLKb);
  /*#p18.HERA*/ wire HERA_FREQ_RST  = nor2(GASE_WAVE_CLKn, GARA.qp_new());
  /*_p18.KYKO*/ wire KYKO_FREQ_RSTn = not1(HERA_FREQ_RST);
  /*_p18.JERA*/ wire JERA_FREQ_RSTo = not1(HERA_FREQ_RST);
  /*_p18.KASO*/ wire KASO_FREQ_RSTp = not1(HERA_FREQ_RST);

  /*_p18.JUTY*/ wire JUTY_FREQ_CLKa = not1(HEFO_FREQ_CLKn);
  /*#p18.KYRU*/ wire KYRU_FREQ_CLKb = not1(KEMU_FREQ_03.qn_new());
  /*#p18.KESE*/ wire KESE_FREQ_CLKc = not1(KENO_FREQ_07.qn_new());

  /*#p18.KUTU*/ KUTU_FREQ_00.dff20(JUTY_FREQ_CLKa,          KYKO_FREQ_RSTn, KOGA_NR33_FREQ00.qn_old());
  /*#p18.KUPE*/ KUPE_FREQ_01.dff20(KUTU_FREQ_00.qp_new(),   KYKO_FREQ_RSTn, JOVY_NR33_FREQ01.qn_old());
  /*_p18.KUNU*/ KUNU_FREQ_02.dff20(KUPE_FREQ_01.qp_new(),   KYKO_FREQ_RSTn, JAXA_NR33_FREQ02.qn_old());
  /*_p18.KEMU*/ KEMU_FREQ_03.dff20(KUNU_FREQ_02.qp_new(),   KYKO_FREQ_RSTn, JEFE_NR33_FREQ03.qn_old());
  /*_p18.KYGU*/ KYGU_FREQ_04.dff20(KYRU_FREQ_CLKb,          JERA_FREQ_RSTo, JYPO_NR33_FREQ04.qn_old());
  /*_p18.KEPA*/ KEPA_FREQ_05.dff20(KYGU_FREQ_04.qp_new(),   JERA_FREQ_RSTo, JOVE_NR33_FREQ05.qn_old());
  /*_p18.KAFO*/ KAFO_FREQ_06.dff20(KEPA_FREQ_05.qp_new(),   JERA_FREQ_RSTo, KANA_NR33_FREQ06.qn_old());
  /*_p18.KENO*/ KENO_FREQ_07.dff20(KAFO_FREQ_06.qp_new(),   JERA_FREQ_RSTo, KOGU_NR33_FREQ07.qn_old());
  /*_p18.KEJU*/ KEJU_FREQ_08.dff20(KESE_FREQ_CLKc,          KASO_FREQ_RSTp, JEMO_NR34_FREQ08.qn_old());
  /*_p18.KEZA*/ KEZA_FREQ_09.dff20(KEJU_FREQ_08.qp_new(),   KASO_FREQ_RSTp, JETY_NR34_FREQ09.qn_old());
  /*_p18.JAPU*/ JAPU_FREQ_10.dff20(KEZA_FREQ_09.qp_new(),   KASO_FREQ_RSTp, JACY_NR34_FREQ10.qn_old());

  /*#p18.HUPA*/ wire HUPA = and2(HUNO_WAVE_CLKo.qp_new(), spu.CERY_2M.qp_new());
  /*#p18.GAFU*/ wire GAFU = nor3(KEBA_APU_RSTp, GARA.qp_new(), HUPA);

  /*#p18.HYFO*/ wire HYFO_CLK = not1(JAPU_FREQ_10.qp_new());
  /*#p18.HUNO*/ HUNO_WAVE_CLKo.dff17(HYFO_CLK, GAFU, HUNO_WAVE_CLKo.qn_old());

  /*#p18.FOTO*/ wire FOTO = and2(FETY_WAVE_LOOP.qp_new(), GASE_WAVE_CLKn);
  /*#p18.ETAN*/ wire ETAN_WAVE_RST = or2(GARA.qp_new(), FETY_WAVE_LOOP.qp_new());
  /*#p18.GYRY*/ wire GYRY_LOOP_RST = nor3(KEBA_APU_RSTp, GARA.qp_new(), FOTO);

  /*#p18.DERO*/ wire DERO_WAVE_CLK  = not1(GASE_WAVE_CLKn);
  /*#p18.EFAR*/ EFAR_WAVE_IDX0.dff17(DERO_WAVE_CLK,           ETAN_WAVE_RST, EFAR_WAVE_IDX0.qn_old());
  /*#p18.ERUS*/ ERUS_WAVE_IDX1.dff17(EFAR_WAVE_IDX0.qn_new(), ETAN_WAVE_RST, ERUS_WAVE_IDX1.qn_old());
  /*#p18.EFUZ*/ EFUZ_WAVE_IDX2.dff17(ERUS_WAVE_IDX1.qn_new(), ETAN_WAVE_RST, EFUZ_WAVE_IDX2.qn_old());
  /*#p18.EXEL*/ EXEL_WAVE_IDX3.dff17(EFUZ_WAVE_IDX2.qn_new(), ETAN_WAVE_RST, EXEL_WAVE_IDX3.qn_old());
  /*#p18.EFAL*/ EFAL_WAVE_IDX4.dff17(EXEL_WAVE_IDX3.qn_new(), ETAN_WAVE_RST, EFAL_WAVE_IDX4.qn_old());
  /*#p18.FETY*/ FETY_WAVE_LOOP.dff17(EFAL_WAVE_IDX4.qn_new(), GYRY_LOOP_RST, FETY_WAVE_LOOP.qn_old());

  //----------
  // Wave ram control signals

  /*#p16.GOMA*/ wire GOMA_APU_RESETn = not1(KEBA_APU_RSTp);
  /*#p16.FOBA*/ FOBA_CH3_RESTART_SYNC.dff17(DOVA_ABCDxxxx, GOMA_APU_RESETn, GAVU_NR34_START.qn_old());

  
  /*#p18.GEDO*/ wire GEDO = and2(FEXU_LEN_DONE.qp_new(), HOTO_NR34_STOP.qn_new()); // schematic had BUFY instead of FEXU?
  /*#p18.FYGO*/ wire FYGO = or3(KEBA_APU_RSTp, GEDO, GUXE_CH3_AMP_ENna.qp_new());
  
  /*#p18.FOZU*/ FOZU.nor_latch(GARA.qp_new(), FYGO);
  /*#p18.EZAS*/ wire EZAS = not1(FOZU.qp_new());
  /*#p18.DORU*/ wire DORU = not1(EZAS);

  /*#p18.CALU*/ wire CALU_APU_RESETn = not1(KEBA_APU_RSTp);
  /*#p18.DAVO*/ DAVO_CH3_ACTIVEp.dff17(spu.AJER_2M.qp_new(), CALU_APU_RESETn, DORU);


  /*#p18.COKA*/ wire COKA_CH3_ACTIVE = not1(DAVO_CH3_ACTIVEp.qn_new());









  // Wave RAM stuff
  /*#p10.ACOM*/ wire ACOM_ADDR_XX3Xn = nand4(AVUN_A07n, ASAD_A06n, BUS_CPU_A05p.out_new(), BUS_CPU_A04p.out_new());
  /*#p10.BARO*/ wire BARO_ADDR_FF3X  = nor2(ACOM_ADDR_XX3Xn, BAKO_ADDR_FFXXn);

  // wave ram control line 1
  /*_p17.BYZA*/ wire BYZA_WAVE_WRp = and2(BOGY_CPU_WRp, BARO_ADDR_FF3X);
  /*#p17.AMYT*/ wire AMYT_WAVE_WRn = not1(BYZA_WAVE_WRp); // to wave ram?

  // wave ram control line 2

  /*#p17.BAMA*/ wire BAMA_APU_RESETn = not1(KEBA_APU_RSTp);
  /*#p17.COZY*/ wire COZY_CLK = not1(AMUK_xBxDxFxH);
  /*#p17.ARUC*/ wire ARUC_CLK = not1(AMUK_xBxDxFxH);
  /*#p17.BUSA*/ BUSA.dff17(AMUK_xBxDxFxH, BAMA_APU_RESETn, GASE_WAVE_CLKn);
  /*#p17.BANO*/ BANO.dff17(COZY_CLK,      BAMA_APU_RESETn, BUSA.qp_old());
  /*#p17.AZUS*/ AZUS.dff17(AMUK_xBxDxFxH, BAMA_APU_RESETn, BANO.qp_old());
  /*_p17.AZET*/ AZET.dff17(ARUC_CLK,      BAMA_APU_RESETn, AZUS.qp_old());
  
  /*#p17.BOKE*/ wire BOKE_CPU_RDa = not1(AGUZ_CPU_RDn);
  /*#p17.BENA*/ wire BENA_CPU_WAVE_RDp = nand2(BOKE_CPU_RDa, BARO_ADDR_FF3X);
  /*#p17.CAZU*/ wire CAZU_CPU_WAVE_RDn = not1(BENA_CPU_WAVE_RDp);
  /*#p17.BETA*/ wire BETA = or3(BYZA_WAVE_WRp, CAZU_CPU_WAVE_RDn, BORY_ABxxxxxH);

  /*#p17.BOXO*/ wire BOXO = nor2(AZUS.qp_new(), AZET.qp_new());
  /*#p17.AZOR*/ wire AZOR = not1(BETA);
  /*#p17.BORU*/ wire BORU = not1(BOXO);
  /*#p17.BUKU*/ wire BUKU = not1(AZOR);
  /*#p17.ATUR*/ wire ATUR_WAVE_RAM_CTRL1n = mux2p(COKA_CH3_ACTIVE, BORU, BUKU);  // to wave ram?
  /*#p17.ALER*/ wire ALER_WAVE_RAM_CTRL1p = not1(ATUR_WAVE_RAM_CTRL1n);

  // wave ram control line 3
  /*#p17.BUTU*/ wire BUTU_SAMPLE_CLK = not1(AZUS.qn_new());
  /*#p18.BENO*/ wire BENO_WAVE_RAM_CTRL3n = mux2p(COKA_CH3_ACTIVE, BUTU_SAMPLE_CLK, CAZU_CPU_WAVE_RDn); // to wave ram?
  /*#p18.ATOK*/ wire ATOK_WAVE_RAM_CTRL3p = not1(BENO_WAVE_RAM_CTRL3n); // to wave ram?

  // wave ram -> sample register

  /*#p17.BEKA*/ wire BEKA_SAMPLE_CLKa = not1(BUTU_SAMPLE_CLK);
  /*#p17.COJU*/ wire COJU_SAMPLE_CLKb = not1(BUTU_SAMPLE_CLK);
  /*#p17.BAJA*/ wire BAJA_SAMPLE_CLKc = not1(BUTU_SAMPLE_CLK);
  /*#p17.BUFE*/ wire BUFE_SAMPLE_CLKd = not1(BUTU_SAMPLE_CLK);

  /*#p17.ACOR*/ wire ACOR_APU_RESETv = not1(KEBA_APU_RSTp);

  /*_p17.CYFO*/ CYFO_SAMPLE_0.dff9(BEKA_SAMPLE_CLKa, ACOR_APU_RESETv, BUS_WAVE_D00p.qp_old());
  /*_p17.CESY*/ CESY_SAMPLE_1.dff9(BUFE_SAMPLE_CLKd, ACOR_APU_RESETv, BUS_WAVE_D01p.qp_old());
  /*_p17.BUDY*/ BUDY_SAMPLE_2.dff9(BAJA_SAMPLE_CLKc, ACOR_APU_RESETv, BUS_WAVE_D02p.qp_old());
  /*_p17.BEGU*/ BEGU_SAMPLE_3.dff9(COJU_SAMPLE_CLKb, ACOR_APU_RESETv, BUS_WAVE_D03p.qp_old());
  /*_p17.CUVO*/ CUVO_SAMPLE_4.dff9(BEKA_SAMPLE_CLKa, ACOR_APU_RESETv, BUS_WAVE_D04p.qp_old());
  /*_p17.CEVO*/ CEVO_SAMPLE_5.dff9(BUFE_SAMPLE_CLKd, ACOR_APU_RESETv, BUS_WAVE_D05p.qp_old());
  /*_p17.BORA*/ BORA_SAMPLE_6.dff9(BAJA_SAMPLE_CLKc, ACOR_APU_RESETv, BUS_WAVE_D06p.qp_old());
  /*_p17.BEPA*/ BEPA_SAMPLE_7.dff9(COJU_SAMPLE_CLKb, ACOR_APU_RESETv, BUS_WAVE_D07p.qp_old());

  // wave ram -> cpu bus

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

  //----------
  // Output

  /*#p17.DATE*/ wire DATE_WAVE_PLAY_D0 = mux2n(EFAR_WAVE_IDX0.qp_new(), CYFO_SAMPLE_0.qp_new(), CUVO_SAMPLE_4.qp_new());
  /*#p17.DAZY*/ wire DAZY_WAVE_PLAY_D1 = mux2n(EFAR_WAVE_IDX0.qp_new(), CESY_SAMPLE_1.qp_new(), CEVO_SAMPLE_5.qp_new());
  /*#p17.CUZO*/ wire CUZO_WAVE_PLAY_D2 = mux2n(EFAR_WAVE_IDX0.qp_new(), BUDY_SAMPLE_2.qp_new(), BORA_SAMPLE_6.qp_new());
  /*#p17.COPO*/ wire COPO_WAVE_PLAY_D3 = mux2n(EFAR_WAVE_IDX0.qp_new(), BEGU_SAMPLE_3.qp_new(), BEPA_SAMPLE_7.qp_new());

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

  /*#p18.GEMY*/ wire GEMY_WAVE_VOL3 = nor2(HUKY_NR32_VOL0.qp_new(), HODY_NR32_VOL1.qp_new());
  /*#p18.GOKA*/ wire GOKA_WAVE_VOL2 = nor2(HUKY_NR32_VOL0.qp_new(), HODY_NR32_VOL1.qn_new());
  /*#p18.GEGE*/ wire GEGE_WAVE_VOL4 = nor2(HUKY_NR32_VOL0.qn_new(), HODY_NR32_VOL1.qp_new());

  /*#p18.EZAG*/ wire EZAG_WAVE_OUT0 = amux3(DATE_WAVE_PLAY_D0, GEGE_WAVE_VOL4, DAZY_WAVE_PLAY_D1, GOKA_WAVE_VOL2, CUZO_WAVE_PLAY_D2, GEMY_WAVE_VOL3);
  /*#p18.EVUG*/ wire EVUG_WAVE_OUT1 = amux3(DAZY_WAVE_PLAY_D1, GEGE_WAVE_VOL4, CUZO_WAVE_PLAY_D2, GOKA_WAVE_VOL2, COPO_WAVE_PLAY_D3, GEMY_WAVE_VOL3);
  /*#p18.DOKY*/ wire DOKY_WAVE_OUT2 = amux2(CUZO_WAVE_PLAY_D2, GEGE_WAVE_VOL4, COPO_WAVE_PLAY_D3, GOKA_WAVE_VOL2);
  /*#p18.DORE*/ wire DORE_WAVE_OUT3 = and2 (COPO_WAVE_PLAY_D3, GEGE_WAVE_VOL4);

  // these go straight to the dac
  /*#p18.BARY*/ wire BARY_WAVE_DAC0 = and2(COKA_CH3_ACTIVE, EZAG_WAVE_OUT0);
  /*#p18.BYKA*/ wire BYKA_WAVE_DAC1 = and2(COKA_CH3_ACTIVE, EVUG_WAVE_OUT1);
  /*#p18.BOPA*/ wire BOPA_WAVE_DAC2 = and2(COKA_CH3_ACTIVE, DOKY_WAVE_OUT2);
  /*#p18.BELY*/ wire BELY_WAVE_DAC3 = and2(COKA_CH3_ACTIVE, DORE_WAVE_OUT3);

  //----------
  // Wave ram address

  /*_p18.BOLE*/ wire BOLE_WAVE_A0 = mux2p(COKA_CH3_ACTIVE, ERUS_WAVE_IDX1.qp_new(), BUS_CPU_A00p.out_new());
  /*_p18.AGYL*/ wire AGYL_WAVE_A1 = mux2p(COKA_CH3_ACTIVE, EFUZ_WAVE_IDX2.qp_new(), BUS_CPU_A01p.out_new());
  /*_p18.AFUM*/ wire AFUM_WAVE_A2 = mux2p(COKA_CH3_ACTIVE, EXEL_WAVE_IDX3.qp_new(), BUS_CPU_A02p.out_new());
  /*_p18.AXOL*/ wire AXOL_WAVE_A3 = mux2p(COKA_CH3_ACTIVE, EFAL_WAVE_IDX4.qp_new(), BUS_CPU_A03p.out_new());





















































  //========================================
  //========================================
  //========================================
  //========================================
  //             CHANNEL 4
  //========================================
  //========================================
  //========================================
  //========================================

  //----------
  // FF20 NR41 - the length register is also the length timer

  /*#p10.DONA*/ wire DONA_ADDR_0000bn = nand4(DYTE_ADDR_xxx0, AFOB_ADDR_xx0x, ABUB_ADDR_x0xx, ACOL_ADDR_0xxx);
  /*#p10.DANU*/ wire DANU_FF20a    = nor2(DONA_ADDR_0000bn, BEZY_ADDR_FF2Xn);
  /*#p19.CAZE*/ wire CAZE_FF20_WRn = nand2(BOGY_CPU_WRp, DANU_FF20a);
  /*#p19.FURU*/ wire FURU_FF20_WRa = not1(CAZE_FF20_WRn);
  /*#p19.DOTU*/ wire DOTU_FF20_WRb = not1(CAZE_FF20_WRn);
  /*#p19.EPEK*/ wire EPEK_FF20_WRc = not1(CAZE_FF20_WRn);

  /*#p19.GAPY*/ wire GAPY_LEN_RSTn = nor3(FURU_FF20_WRa, KEBA_APU_RSTp, GONE.qp_new());

  /*#p19.DODA*/ wire DODA_LEN_CLKn = nor3(FUGO.qp_new(), BUFY_CLK_256, CUNY_NR44_STOP.qp_new());
  /*#p19.CUWA*/ wire CUWA_LEN_CLKa = not1(DODA_LEN_CLKn);
  /*#p19.DOPU*/ wire DOPU_NR41_LEN3b = not1(CEDO.qn_new());

  /*#p19.DANO*/ DANO.dff20(CUWA_LEN_CLKa,   DOTU_FF20_WRb, BUS_CPU_D00p.qp_new());
  /*#p19.FAVY*/ FAVY.dff20(DANO.qp_new(),   DOTU_FF20_WRb, BUS_CPU_D01p.qp_new());
  /*#p19.DENA*/ DENA.dff20(FAVY.qp_new(),   DOTU_FF20_WRb, BUS_CPU_D02p.qp_new());
  /*#p19.CEDO*/ CEDO.dff20(DENA.qp_new(),   DOTU_FF20_WRb, BUS_CPU_D03p.qp_new());
  /*#p19.FYLO*/ FYLO.dff20(DOPU_NR41_LEN3b, EPEK_FF20_WRc, BUS_CPU_D04p.qp_new());
  /*#p19.EDOP*/ EDOP.dff20(FYLO.qp_new(),   EPEK_FF20_WRc, BUS_CPU_D05p.qp_new());
  /*#p19.FUGO*/ FUGO.dff17(EDOP.qn_new(),   GAPY_LEN_RSTn, FUGO.qn_old());

  //----------
  // FF21

  /*#p10.DEWA*/ wire DEWA_ADDR_0001bn = nand4(DOSO_ADDR_xxx1, AFOB_ADDR_xx0x, ABUB_ADDR_x0xx, ACOL_ADDR_0xxx);
  /*#p10.COVO*/ wire COVO_FF21p    = nor2(DEWA_ADDR_0001bn, BEZY_ADDR_FF2Xn);
  /*#p19.BOFY*/ wire BOFY_FF21n    = not1(COVO_FF21p);
  /*#p19.GONY*/ wire GONY_FF21n    = not1(COVO_FF21p);
  /*#p19.DACO*/ wire DACO_FF21_WRp = and2(BOGY_CPU_WRp, COVO_FF21p);
  /*#p19.GOKO*/ wire GOKO_FF21_WRp = and2(COVO_FF21p, BOGY_CPU_WRp);
  /*#p19.BOXE*/ wire BOXE_FF21_RDn = or2(BOFY_FF21n, AGUZ_CPU_RDn);
  /*#p19.HASU*/ wire HASU_FF21_RDn = or2(GONY_FF21n, AGUZ_CPU_RDn);

  /*#p19.FEXO*/ wire FEXO_RSTn = not1(KEBA_APU_RSTp);

  /*#p19.DYKE*/ wire DYKE_FF21_WRn = not1(DACO_FF21_WRp);
  /*#p19.FUPA*/ wire FUPA_FF21_WRn = not1(GOKO_FF21_WRp);
  
  /*_p19.EMOK*/ EMOK_NR42_ENV_TIMER0 .dff9(DYKE_FF21_WRn, FEXO_RSTn, BUS_CPU_D00p.qp_old());
  /*_p19.ETYJ*/ ETYJ_NR42_ENV_TIMER1 .dff9(DYKE_FF21_WRn, FEXO_RSTn, BUS_CPU_D01p.qp_old());
  /*_p19.EZYK*/ EZYK_NR42_ENV_TIMER2 .dff9(DYKE_FF21_WRn, FEXO_RSTn, BUS_CPU_D02p.qp_old());
  /*_p19.GEKY*/ GEKY_NR42_ENV_DIR    .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D03p.qp_old());
  /*_p19.GARU*/ GARU_NR42_ENV_VOL0   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D04p.qp_old());
  /*_p19.GOKY*/ GOKY_NR42_ENV_VOL1   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D05p.qp_old());
  /*_p19.GOZO*/ GOZO_NR42_ENV_VOL2   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D06p.qp_old());
  /*_p19.GEDU*/ GEDU_NR42_ENV_VOL3   .dff9(FUPA_FF21_WRn, FEXO_RSTn, BUS_CPU_D07p.qp_old());

  /*#p19.DEMY*/ triwire DEMY = tri6_nn(BOXE_FF21_RDn, EMOK_NR42_ENV_TIMER0.qp_new());
  /*#p19.COCE*/ triwire COCE = tri6_nn(BOXE_FF21_RDn, ETYJ_NR42_ENV_TIMER1.qp_new());
  /*#p19.CUZU*/ triwire CUZU = tri6_nn(BOXE_FF21_RDn, EZYK_NR42_ENV_TIMER2.qp_new());
  /*#p19.GOME*/ triwire GOME = tri6_nn(HASU_FF21_RDn, GEKY_NR42_ENV_DIR.qp_new());
  /*#p19.HEDA*/ triwire HEDA = tri6_nn(HASU_FF21_RDn, GARU_NR42_ENV_VOL0.qp_new());
  /*#p19.GODU*/ triwire GODU = tri6_nn(HASU_FF21_RDn, GOKY_NR42_ENV_VOL1.qp_new());
  /*#p19.HOGE*/ triwire HOGE = tri6_nn(HASU_FF21_RDn, GOZO_NR42_ENV_VOL2.qp_new());
  /*#p19.HACU*/ triwire HACU = tri6_nn(HASU_FF21_RDn, GEDU_NR42_ENV_VOL3.qp_new());

  //----------
  // FF22

  /*#p09.DAPA*/ wire DAPA_RSTn = not1(KEBA_APU_RSTp); // bavu crosses over this? why is it 3-rung?

  /*#p10.DOFA*/ wire DOFA_ADDR_0010p = and4(ACOL_ADDR_0xxx, ABUB_ADDR_x0xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
  /*#p10.EKEZ*/ wire EKEZ_FF22p = and2(CONA_ADDR_FF2Xp, DOFA_ADDR_0010p);
  /*#p19.KOKU*/ wire KOKU_FF22n = not1(EKEZ_FF22p);
  /*#p19.GUGO*/ wire GUGO_FF22n = not1(EKEZ_FF22p);

  /*#p19.HUMO*/ wire HUMO_FF22_WRp = and2(BOGY_CPU_WRp, EKEZ_FF22p);
  /*#p19.GETU*/ wire GETU_FF22_WRp = and2(BOGY_CPU_WRp, EKEZ_FF22p);

  /*#p19.KEKA*/ wire KEKA_FF22_RDn = or2(KOKU_FF22n, AGUZ_CPU_RDn);

  /*#p19.KAGE*/ wire KAGE_CPU_RDp = not1(AGUZ_CPU_RDn);
  /*#p19.JORA*/ wire JORA_FF22_RDn = nand2(KAGE_CPU_RDp, EKEZ_FF22p);
  /*#p19.HEZE*/ wire HEZE_FF22_RDn = or2(GUGO_FF22n, AGUZ_CPU_RDn);

  /*#p09.KAME*/ wire KAME_RSTn = not1(KEBA_APU_RSTp);
  /*#p19.HYNE*/ wire HYNE_RSTn = not1(KEBA_APU_RSTp);

  /*#p19.HOVA*/ wire HOVA_FF22_WRn = not1(HUMO_FF22_WRp);
  /*#p19.HOSO*/ wire HOSO_FF22_WRn = nand2(EKEZ_FF22p, BOGY_CPU_WRp);
  /*#p19.EFUG*/ wire EFUG_FF22_WRn = not1(GETU_FF22_WRp);
  /*#p19.JARE*/ JARE_NR43_DIV0  .dff9(HOVA_FF22_WRn, KAME_APU_RSTn, BUS_CPU_D00p.qp_new());
  /*#p19.JERO*/ JERO_NR43_DIV1  .dff9(HOVA_FF22_WRn, KAME_APU_RSTn, BUS_CPU_D01p.qp_new());
  /*#p19.JAKY*/ JAKY_NR43_DIV2  .dff9(HOVA_FF22_WRn, KAME_APU_RSTn, BUS_CPU_D02p.qp_new());
  /*#p19.JAMY*/ JAMY_NR43_MODE  .dff9(HOSO_FF22_WRn, HYNE_RSTn,     BUS_CPU_D03p.qp_new());
  /*#p19.FETA*/ FETA_NR43_FREQ0 .dff9(EFUG_FF22_WRn, DAPA_RSTn,     BUS_CPU_D04p.qp_new());
  /*#p19.FYTO*/ FYTO_NR43_FREQ1 .dff9(EFUG_FF22_WRn, DAPA_RSTn,     BUS_CPU_D05p.qp_new());
  /*_p19.GOGO*/ GOGO_NR43_FREQ2 .dff9(EFUG_FF22_WRn, DAPA_RSTn,     BUS_CPU_D06p.qp_new());
  /*_p19.GAFO*/ GAFO_NR43_FREQ3 .dff9(EFUG_FF22_WRn, DAPA_RSTn,     BUS_CPU_D07p.qp_new());

  /*#p19.KAMO*/ triwire KAMO = tri6_nn(KEKA_FF22_RDn, JARE_NR43_DIV0.qp_new());
  /*#p19.KAKU*/ triwire KAKU = tri6_nn(KEKA_FF22_RDn, JERO_NR43_DIV1.qp_new());
  /*_p19.KYRO*/ triwire KYRO = tri6_nn(KEKA_FF22_RDn, JAKY_NR43_DIV2.qp_new());
  /*_p19.KETA*/ triwire KETA = tri6_nn(JORA_FF22_RDn, JAMY_NR43_MODE.qp_new());
  /*_p19.GEDA*/ triwire GEDA = tri6_nn(HEZE_FF22_RDn, FETA_NR43_FREQ0.qp_new());
  /*_p19.GYPE*/ triwire GYPE = tri6_nn(HEZE_FF22_RDn, FYTO_NR43_FREQ1.qp_new());
  /*_p19.GAKA*/ triwire GAKA = tri6_nn(HEZE_FF22_RDn, GOGO_NR43_FREQ2.qp_new());
  /*_p19.HAPY*/ triwire HAPY = tri6_nn(HEZE_FF22_RDn, GAFO_NR43_FREQ3.qp_new());

  //----------
  // FF23. Some weird debug voodoo here.

  /*#p19.BYLO*/ wire BYLO_CPU_RDp = not1(AGUZ_CPU_RDn);
  /*#p10.DUFE*/ wire DUFE_ADDR_0011n = nand4(DOSO_ADDR_xxx1, DUPA_ADDR_xx1x, ABUB_ADDR_x0xx, ACOL_ADDR_0xxx);

  /*#p10.CUGE*/ wire CUGE_FF23p    = nor2(DUFE_ADDR_0011n, BEZY_ADDR_FF2Xn);
  /*#p19.BARE*/ wire BARE_FF23_RDn = nand2(CUGE_FF23p, BYLO_CPU_RDp);

  /*#p19.CABE*/ wire CABE_RSTn = not1(KEBA_APU_RSTp);

  /*#p19.DULU*/ wire DULU_FF23_WRn = nand2(ANUJ_CPU_WR_WEIRD, CUGE_FF23p);
  /*#p19.CUNY*/ CUNY_NR44_STOP.dff9(DULU_FF23_WRn, CABE_RSTn, BUS_CPU_D06p.qp_new());

  /*#p20.GUZY*/ wire GUZY_NR44_START_RST = nor2(KEBA_APU_RSTp, GYSU.qp_new());
  /*#p19.FOXE*/ wire FOXE_FF23_WRo = nand2(BOGY_CPU_WRp, CUGE_FF23p);
  /*#p19.HOGA*/ HOGA_NR44_START.dff9(FOXE_FF23_WRo, GUZY_NR44_START_RST, BUS_CPU_D07p.qp_old());

  /*#p19.CURY*/ triwire CURY = tri6_nn(BARE_FF23_RDn, CUNY_NR44_STOP.qp_new());

  //----------
  // Debug

  /*#p20.COSA*/ wire COSA_CPU_RDp = not1(AGUZ_CPU_RDn);
  /*#p20.CEPY*/ wire NR44_STOPn = not1(CUNY_NR44_STOP.qn_new());

  /*#p20.DYRY*/ wire DYRY_DBG = and2(CUNY_NR44_STOP.qn_new(), EDEK_NR52_DBG_APUn);
  /*_p20.COMO*/ wire COMO_DBG = and2(DYRY_DBG, COSA_CPU_RDp);
  /*_p20.BAGU*/ wire BAGU_DBG = nand2(CUGE_FF23p, COMO_DBG);
  ///*_p20.BEFA*/ wire BEFA_DBG = not1(CARY);

  /*_p20.ATEL*/ triwire ATEL = tri6_nn(BAGU_DBG, BEZY_ADDR_FF2Xn);

  //----------
  // Control

  /*#p20.GEVY*/ wire GEVY_CH4_AMP_ENn = nor5(GEKY_NR42_ENV_DIR.qn_new(), GARU_NR42_ENV_VOL0.qn_new(), GOKY_NR42_ENV_VOL1.qn_new(), GOZO_NR42_ENV_VOL2.qn_new(), GEDU_NR42_ENV_VOL3.qn_new());

  /*#p20.FEBY*/ wire FEBY_RSTn = not1(KEBA_APU_RSTp);
  /*#p20.GASO*/ wire GASO_RSTn = not1(KEBA_APU_RSTp);

  /*#p20.GYSU*/ GYSU.dff17(DOVA_ABCDxxxx, GASO_RSTn, HOGA_NR44_START.qn_old());

  /*#p20.EFOT*/ wire EFOT_CH4_STOP    = and2(CUNY_NR44_STOP.qn_new(), FUGO.qp_new());
  /*#p20.FEGY*/ wire FEGY_CH4_OFF     = or3(KEBA_APU_RSTp, EFOT_CH4_STOP, GEVY_CH4_AMP_ENn);

  /*#p20.GENA*/ GENA_CH4_ACTIVE.nor_latch(GONE.qp_new(), FEGY_CH4_OFF);

  /*#p20.FALE*/ wire FALE_RESTART_RSTn = nor2(KEBA_APU_RSTp, GORA.qp_new());
  /*#p20.HELU*/ wire HELU_RESTART_RSTp  = not1(FALE_RESTART_RSTn);

  /*#p20.HAZO*/ HAZO.nor_latch(HELU_RESTART_RSTp, GYSU.qp_new());

  // one of these is wrong, right now we would stop the div clock when ch4 active
  // fixed kyku but still might be a polarity error?

  /*#p20.GONE*/ GONE.dff17(HAMA_CLK_512K, FALE_RESTART_RSTn, HAZO.qn_new());
  /*#p20.GORA*/ GORA.dff17(HAMA_CLK_512K, FEBY_RSTn,         GONE.qp_old());
  /*#p20.GATY*/ GATY.dff17(HAMA_CLK_512K, FEBY_RSTn,         GORA.qp_old());

  /*#p20.HERY*/ wire HERY_DIV_GATE1 = nor2(GEVY_CH4_AMP_ENn, KEBA_APU_RSTp);
  /*#p20.HAPU*/ wire HAPU_DIV_GATE2 = not1(GATY.qp_new());
  /*#p20.JERY*/ JERY.nand_latch(HERY_DIV_GATE1, HAPU_DIV_GATE2);

  /*#p20.KYKU*/ wire KYKU_DIV_CLKb  = or2(JERY.qp_new(), spu.JESO_CLK_512K.qp_new());
  /*#p20.KONY*/ wire KONY_DIV_CLKn  = not1(KYKU_DIV_CLKb);
  /*#p20.GOFU*/ wire GOFU_DIV_LOADn = nor2(GONE.qp_new(), GARY.qp_new());
  /*#p20.HUCE*/ wire HUCE_DIV_LOAD  = not1(GOFU_DIV_LOADn);

  /*#p20.KANU*/ wire KANU_DIV_CLKa  = not1(KONY_DIV_CLKn);
  /*#p20.JYCO*/ JYCO.dff20(KANU_DIV_CLKa, HUCE_DIV_LOAD, JARE_NR43_DIV0.qp_old());
  /*#p20.JYRE*/ JYRE.dff20(JYCO.qp_new(), HUCE_DIV_LOAD, JERO_NR43_DIV1.qp_old());
  /*_p20.JYFU*/ JYFU.dff20(JYRE.qp_new(), HUCE_DIV_LOAD, JAKY_NR43_DIV2.qp_old());

  //----------
  // Frequency timer

  /*#p20.GUNY*/ wire GUNY_FREQ_GATE_RSTn = nor2(KEBA_APU_RSTp, GONE.qp_new());
  /*#p20.GYBA*/ wire GYBA_FREQ_GATE_CLK  = not1(BAVU_1M);

  /*#p20.HYNO*/ wire HYNO_DIV_MAX = or3(JYFU.qp_new(), JYRE.qp_new(), JYCO.qp_new());
  /*#p20.GARY*/ GARY.dff17(GYBA_FREQ_GATE_CLK, GUNY_FREQ_GATE_RSTn, HYNO_DIV_MAX);
  /*#p20.CARY*/ wire CARY_FREQ_CLK = and2(BAVU_1M, GARY.qp_new());

  /*#p20.CEXO*/ CEXO_FREQ_00.dff17(CARY_FREQ_CLK,         DAPA_RSTn, CEXO_FREQ_00.qn_old());
  /*_p20.DEKO*/ DEKO_FREQ_01.dff17(CEXO_FREQ_00.qn_new(), DAPA_RSTn, DEKO_FREQ_01.qn_old());
  /*_p20.EZEF*/ EZEF_FREQ_02.dff17(DEKO_FREQ_01.qn_new(), DAPA_RSTn, EZEF_FREQ_02.qn_old());
  /*_p20.EPOR*/ EPOR_FREQ_03.dff17(EZEF_FREQ_02.qn_new(), DAPA_RSTn, EPOR_FREQ_03.qn_old());
  /*_p20.DURE*/ DURE_FREQ_04.dff17(EPOR_FREQ_03.qn_new(), DAPA_RSTn, DURE_FREQ_04.qn_old());
  /*_p20.DALE*/ DALE_FREQ_05.dff17(DURE_FREQ_04.qn_new(), DAPA_RSTn, DALE_FREQ_05.qn_old());
  /*_p20.DOKE*/ DOKE_FREQ_06.dff17(DALE_FREQ_05.qn_new(), DAPA_RSTn, DOKE_FREQ_06.qn_old());
  /*_p20.DEMO*/ DEMO_FREQ_07.dff17(DOKE_FREQ_06.qn_new(), DAPA_RSTn, DEMO_FREQ_07.qn_old());
  /*_p20.DOSE*/ DOSE_FREQ_08.dff17(DEMO_FREQ_07.qn_new(), DAPA_RSTn, DOSE_FREQ_08.qn_old());
  /*_p20.DETE*/ DETE_FREQ_09.dff17(DOSE_FREQ_08.qn_new(), DAPA_RSTn, DETE_FREQ_09.qn_old());
  /*_p20.ERUT*/ ERUT_FREQ_10.dff17(DETE_FREQ_09.qn_new(), DAPA_RSTn, ERUT_FREQ_10.qn_old());
  /*_p20.DOTA*/ DOTA_FREQ_11.dff17(ERUT_FREQ_10.qn_new(), DAPA_RSTn, DOTA_FREQ_11.qn_old());
  /*_p20.DERE*/ DERE_FREQ_12.dff17(DOTA_FREQ_11.qn_new(), DAPA_RSTn, DERE_FREQ_12.qn_old());
  /*_p20.ESEP*/ ESEP_FREQ_13.dff17(DERE_FREQ_12.qn_new(), DAPA_RSTn, ESEP_FREQ_13.qn_old());

  //----------
  // The actual LFSR

  // 14-to-1 mux to select LFSR clock

  /*_p20.EMOF*/ wire EMOF_LFSR_CLK_MUX_7 = nor3(FETA_NR43_FREQ0.qn_new(), FYTO_NR43_FREQ1.qn_new(), GOGO_NR43_FREQ2.qn_new());
  /*_p20.ELAR*/ wire ELAR_LFSR_CLK_MUX_6 = nor3(FETA_NR43_FREQ0.qn_new(), FYTO_NR43_FREQ1.qn_new(), GOGO_NR43_FREQ2.qp_new());
  /*_p20.DUDU*/ wire DUDU_LFSR_CLK_MUX_5 = nor3(FETA_NR43_FREQ0.qn_new(), FYTO_NR43_FREQ1.qp_new(), GOGO_NR43_FREQ2.qn_new());
  /*_p20.ETAT*/ wire ETAT_LFSR_CLK_MUX_4 = nor3(FETA_NR43_FREQ0.qn_new(), FYTO_NR43_FREQ1.qp_new(), GOGO_NR43_FREQ2.qp_new());
  /*_p20.FURA*/ wire FURA_LFSR_CLK_MUX_3 = nor3(FETA_NR43_FREQ0.qp_new(), FYTO_NR43_FREQ1.qn_new(), GOGO_NR43_FREQ2.qn_new());
  /*_p20.ETAR*/ wire ETAR_LFSR_CLK_MUX_2 = nor3(FETA_NR43_FREQ0.qp_new(), FYTO_NR43_FREQ1.qn_new(), GOGO_NR43_FREQ2.qp_new());
  /*_p20.EVER*/ wire EVER_LFSR_CLK_MUX_1 = nor3(FETA_NR43_FREQ0.qp_new(), FYTO_NR43_FREQ1.qp_new(), GOGO_NR43_FREQ2.qn_new());
  /*_p20.ETOV*/ wire ETOV_LFSR_CLK_MUX_0 = nor3(FETA_NR43_FREQ0.qp_new(), FYTO_NR43_FREQ1.qp_new(), GOGO_NR43_FREQ2.qp_new());

  /*_p20.ETYR*/ wire ETYR_LFSR_CLK_MUX_A = amux6(ESEP_FREQ_13.qp_new(), DUDU_LFSR_CLK_MUX_5,
                                                  DERE_FREQ_12.qp_new(), ETAT_LFSR_CLK_MUX_4,
                                                  DOTA_FREQ_11.qp_new(), FURA_LFSR_CLK_MUX_3,
                                                  ERUT_FREQ_10.qp_new(), ETAR_LFSR_CLK_MUX_2,
                                                  DETE_FREQ_09.qp_new(), EVER_LFSR_CLK_MUX_1,
                                                  DOSE_FREQ_08.qp_new(), ETOV_LFSR_CLK_MUX_0);

  /*_p20.ELYX*/ wire ELYX_LFSR_CLK_MUX_B = amux4(DEMO_FREQ_07.qp_new(), EMOF_LFSR_CLK_MUX_7,
                                                  DOKE_FREQ_06.qp_new(), ELAR_LFSR_CLK_MUX_6,
                                                  DALE_FREQ_05.qp_new(), DUDU_LFSR_CLK_MUX_5,
                                                  DURE_FREQ_04.qp_new(), ETAT_LFSR_CLK_MUX_4);

  /*_p20.DARY*/ wire DARY_LFSR_CLK_MUX_C = amux4(EPOR_FREQ_03.qp_new(), FURA_LFSR_CLK_MUX_3,
                                                  EZEF_FREQ_02.qp_new(), ETAR_LFSR_CLK_MUX_2,
                                                  DEKO_FREQ_01.qp_new(), EVER_LFSR_CLK_MUX_1,
                                                  CEXO_FREQ_00.qp_new(), ETOV_LFSR_CLK_MUX_0);

  /*_p20.ERYF*/ wire ERYF_LFSR_CLK_MUX_D = or2(ELYX_LFSR_CLK_MUX_B, DARY_LFSR_CLK_MUX_C);

  /*#p20.GEPO*/ wire GEPO_LFSR_RSTa = or2(GONE.qp_new(), KEBA_APU_RSTp);
  /*#p20.GOGE*/ wire GOGE_LFSR_RSTn = not1(GEPO_LFSR_RSTa);


  /*#p20.FEME*/ wire FEME_LFSR_CLKp = mux2p(GAFO_NR43_FREQ3.qn_new(), ETYR_LFSR_CLK_MUX_A, ERYF_LFSR_CLK_MUX_D);
  /*#p20.JYJA*/ wire JYJA_LFSR_CLKn = not1(FEME_LFSR_CLKp);
  /*#p20.GUFA*/ wire GUFA_LFSR_CLKn = not1(FEME_LFSR_CLKp);
  /*#p20.GYVE*/ wire GYVE_LFSR_CLKp = not1(GUFA_LFSR_CLKn);
  /*#p20.KARA*/ wire KARA_LFSR_CLKn = not1(GYVE_LFSR_CLKp);
  /*#p20.KOPA*/ wire KOPA_LFSR_CLKp = not1(KARA_LFSR_CLKn);

  /*#p20.HURA*/ wire HURA_LFSR_IN = xor2(HEZU_LFSR_15.qp_old(), HYRO_LFSR_14.qp_old());
  /*#p20.JOTO*/ JOTO_LFSR_00.dff17(JYJA_LFSR_CLKn, GOGE_LFSR_RSTn, HURA_LFSR_IN);
  /*#p20.KOMU*/ KOMU_LFSR_01.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, JOTO_LFSR_00.qp_new());
  /*#p20.KETU*/ KETU_LFSR_02.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, KOMU_LFSR_01.qp_new());
  /*#p20.KUTA*/ KUTA_LFSR_03.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, KETU_LFSR_02.qp_new());
  /*#p20.KUZY*/ KUZY_LFSR_04.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, KUTA_LFSR_03.qp_new());
  /*#p20.KYWY*/ KYWY_LFSR_05.dff17(KOPA_LFSR_CLKp, GOGE_LFSR_RSTn, KUZY_LFSR_04.qp_new());
  /*#p20.JAJU*/ JAJU_LFSR_06.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, KYWY_LFSR_05.qp_new());
  /*#p20.HAPE*/ HAPE_LFSR_07.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, JAJU_LFSR_06.qp_new());
  /*#p20.JUXE*/ JUXE_LFSR_08.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, HAPE_LFSR_07.qp_new());

  /*#p20.KAVU*/ wire KAVU_LFSR_FB = amux2(JOTO_LFSR_00.qp_old(), JAMY_NR43_MODE.qn_old(), JAMY_NR43_MODE.qp_old(), JUXE_LFSR_08.qp_old());
  /*#p20.JEPE*/ JEPE_LFSR_09.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, KAVU_LFSR_FB);
  /*#p20.JAVO*/ JAVO_LFSR_10.dff17(GYVE_LFSR_CLKp, GOGE_LFSR_RSTn, JEPE_LFSR_09.qp_new());
  /*#p20.HEPA*/ HEPA_LFSR_11.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, JAVO_LFSR_10.qp_new());
  /*#p20.HORY*/ HORY_LFSR_12.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, HEPA_LFSR_11.qp_new());
  /*#p20.HENO*/ HENO_LFSR_13.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, HORY_LFSR_12.qp_new());
  /*#p20.HYRO*/ HYRO_LFSR_14.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, HENO_LFSR_13.qp_new());
  /*_p20.HEZU*/ HEZU_LFSR_15.dff17(FEME_LFSR_CLKp, GOGE_LFSR_RSTn, HYRO_LFSR_14.qp_new());

  //----------
  // Env

  /*#p20.FOWA*/ wire FOWA_ENV_OFF = nor3(EMOK_NR42_ENV_TIMER0.qn_new(), ETYJ_NR42_ENV_TIMER1.qn_new(), EZYK_NR42_ENV_TIMER2.qn_new());

  {
    /*#p20.ALOP*/ wire ALOP_CLK_128 = not1(BYFE_CLK_128);
    /*#p20.BOKY*/ wire BOKY_RSTn = not1(KEBA_APU_RSTp);
    /*#p20.ABEL*/ ABEL_CLK_64.dff17(ALOP_CLK_128, BOKY_RSTn, ABEL_CLK_64.qn_old());
    /*#p20.BAWA*/ wire BAWA_ENV_TIMER_CLKn  = not1(ABEL_CLK_64.qp_new());
    /*#p20.BUXO*/ wire BUXO_ENV_TIMER_CLKb  = not1(BAWA_ENV_TIMER_CLKn);
    /*#p20.ENEC*/ wire ENEC_ENV_TIMER_LOADn = nor2(GONE.qp_new(), FOSY_ENV_PULSE.qp_new());
    /*#p20.DAPY*/ wire DAPY_ENV_TIMER_LOAD  = not1(ENEC_ENV_TIMER_LOADn);
    /*#p20.CUNA*/ CUNA_ENV_TIMER0.dff20(BUXO_ENV_TIMER_CLKb,      DAPY_ENV_TIMER_LOAD, EMOK_NR42_ENV_TIMER0.qp_old());
    /*#p20.COFE*/ COFE_ENV_TIMER1.dff20(CUNA_ENV_TIMER0.qp_new(), DAPY_ENV_TIMER_LOAD, ETYJ_NR42_ENV_TIMER1.qp_old());
    /*#p20.DOGO*/ DOGO_ENV_TIMER2.dff20(COFE_ENV_TIMER1.qp_new(), DAPY_ENV_TIMER_LOAD, EZYK_NR42_ENV_TIMER2.qp_old());
  }


  // Generates a 1 usec pulse when the env timer hits 111
  /*#p20.GEXE*/ wire GEXE_ENV_PULSEn     = not1(FOSY_ENV_PULSE.qp_new());
  /*#p20.HURY*/ wire HURY_ENV_PULSE_RST1 = nor2(HORU_CLK_512, GEXE_ENV_PULSEn);
  /*#p20.GOPA*/ wire GOPA_ENV_PULSE_RST2 = nor4(HURY_ENV_PULSE_RST1, FOWA_ENV_OFF, GONE.qp_new(), KEBA_APU_RSTp);

  /*#p20.DARO*/ wire DARO_ENV_BOT  = nor5 (GEKY_NR42_ENV_DIR.qn_new(), FEKO_VOL0.qp_new(), FATY_VOL1.qp_new(), FERU_VOL2.qp_new(), FYRO_VOL3.qp_new());
  /*#p20.CUTY*/ wire CUTY_ENV_TOPn = nand5(GEKY_NR42_ENV_DIR.qn_new(), FEKO_VOL0.qp_new(), FATY_VOL1.qp_new(), FERU_VOL2.qp_new(), FYRO_VOL3.qp_new());
  /*#p20.DUBO*/ wire DUBO_ENV_TOPa = not1(CUTY_ENV_TOPn);
  /*#p20.EVUR*/ wire EVUR_ENV_MAX  = or2(DARO_ENV_BOT, DUBO_ENV_TOPa);

  /*#p20.EMET*/ wire EMET_ENV_STOP_RST = nor2(GONE.qp_new(), KEBA_APU_RSTp);
  {
    /*#p20.EJEX*/ wire EJEX_ENV_TIMER_MAX   = or3(DOGO_ENV_TIMER2.qp_new(), COFE_ENV_TIMER1.qp_new(), CUNA_ENV_TIMER0.qp_new());
    /*#p20.FOSY*/ FOSY_ENV_PULSE.dff17(HORU_CLK_512,            GOPA_ENV_PULSE_RST2, EJEX_ENV_TIMER_MAX);
    /*#p20.FYNO*/ FYNO_ENV_STOP .dff17(FOSY_ENV_PULSE.qp_new(), EMET_ENV_STOP_RST,   EVUR_ENV_MAX);
  }

  /*#p20.ENUR*/ wire ENUR_ENV_CLK1 = or2(KEBA_APU_RSTp, GONE.qp_new());

  /*#p20.EROX*/ EROX_ENV_CLK2.nor_latch(FYNO_ENV_STOP.qp_new(), ENUR_ENV_CLK1);

  /*#p20.FELO*/ wire FELO_ENV_CLK  = or3(FOSY_ENV_PULSE.qp_new(), FOWA_ENV_OFF, EROX_ENV_CLK2.qp_new());

  /*#p20.FOLE*/ wire FOLE_VOL_CLK0 = amux2(GEKY_NR42_ENV_DIR.qn_new(), FELO_ENV_CLK,       FELO_ENV_CLK,       GEKY_NR42_ENV_DIR.qp_new());
  /*#p20.ETEF*/ wire ETEF_VOL_CLK1 = amux2(GEKY_NR42_ENV_DIR.qn_new(), FEKO_VOL0.qp_new(), FEKO_VOL0.qn_new(), GEKY_NR42_ENV_DIR.qp_new());
  /*#p20.EDYF*/ wire EDYF_VOL_CLK2 = amux2(GEKY_NR42_ENV_DIR.qn_new(), FATY_VOL1.qp_new(), FATY_VOL1.qn_new(), GEKY_NR42_ENV_DIR.qp_new());
  /*#p20.ELAF*/ wire ELAF_VOL_CLK3 = amux2(GEKY_NR42_ENV_DIR.qn_new(), FERU_VOL2.qp_new(), FERU_VOL2.qn_new(), GEKY_NR42_ENV_DIR.qp_new());

  /*#p20.FEKO*/ FEKO_VOL0.dff20(FOLE_VOL_CLK0, GONE.qp_new(), GARU_NR42_ENV_VOL0.qn_old());
  /*#p20.FATY*/ FATY_VOL1.dff20(ETEF_VOL_CLK1, GONE.qp_new(), GOKY_NR42_ENV_VOL1.qn_old());
  /*#p20.FERU*/ FERU_VOL2.dff20(EDYF_VOL_CLK2, GONE.qp_new(), GOZO_NR42_ENV_VOL2.qn_old());
  /*#p20.FYRO*/ FYRO_VOL3.dff20(ELAF_VOL_CLK3, GONE.qp_new(), GEDU_NR42_ENV_VOL3.qn_old());

  //----------
  // Output

  /*#p20.GAME*/ wire GAME_LFSR_OUT = and2(GENA_CH4_ACTIVE.qp_new(), HEZU_LFSR_15.qp_new());
  /*#p20.EZUL*/ wire EZUL_CH4_BIT_MUX = mux2p(DYRY_DBG, FEME_LFSR_CLKp, GAME_LFSR_OUT);
  /*#p20.CEPY*/ wire CEPY_NR44_STOPn = not1(CUNY_NR44_STOP.qn_new());
  /*#p20.COTE*/ wire COTE_DBG_CH4_MUTE = and2(CEPY_NR44_STOPn, EDEK_NR52_DBG_APUn);
  /*#p20.DATO*/ wire DATO_CH4_RAW_BIT  = or2(EZUL_CH4_BIT_MUX, COTE_DBG_CH4_MUTE);

  /*#p20.AKOF*/ wire AKOF_CH4_DAC0 = and2(FEKO_VOL0.qp_new(), DATO_CH4_RAW_BIT);
  /*#p20.BYZY*/ wire BYZY_CH4_DAC1 = and2(FATY_VOL1.qp_new(), DATO_CH4_RAW_BIT);
  /*#p20.APYR*/ wire APYR_CH4_DAC2 = and2(FERU_VOL2.qp_new(), DATO_CH4_RAW_BIT);
  /*#p20.BOZA*/ wire BOZA_CH4_DAC3 = and2(FYRO_VOL3.qp_new(), DATO_CH4_RAW_BIT);























































































  //----------
  // FF10 NR10

  /*#p10.DUPO*/ wire DUPO_ADDR_0000n = nand4(ACOL_ADDR_0xxx, ABUB_ADDR_x0xx, AFOB_ADDR_xx0x, DYTE_ADDR_xxx0);
  /*#p10.DYVA*/ wire DYVA_ADDR_FF10p = nor2(BANU_ADDR_FF1Xn, DUPO_ADDR_0000n);
  /*#p11.BUZE*/ wire BUZE_ADDR_FF10n = not1(DYVA_ADDR_FF10p);

  /*#p10.DAMY*/ wire DAMY_ADDR_0010n = nand4(ACOL_ADDR_0xxx, ABUB_ADDR_x0xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
  /*#p10.EDAF*/ wire EDAF_ADDR_FF12p = nor2(BANU_ADDR_FF1Xn, DAMY_ADDR_0010n);

  /*#p10.ETUF*/ wire ETUF_ADDR_0011n = nand4(ACOL_ADDR_0xxx, ABUB_ADDR_x0xx, DUPA_ADDR_xx1x, DOSO_ADDR_xxx1);
  /*#p10.DECO*/ wire DECO_ADDR_FF13p = nor2(BANU_ADDR_FF1Xn, ETUF_ADDR_0011n);
  /*#p11.CACA*/ wire CACA_ADDR_FF13n = not1(DECO_ADDR_FF13p);

  /*#p10.ESOT*/ wire ESOT_ADDR_0100n = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, AFOB_ADDR_xx0x, DYTE_ADDR_xxx0);
  /*#p10.DUJA*/ wire DUJA_ADDR_FF14p = nor2(BANU_ADDR_FF1Xn, ESOT_ADDR_0100n);
  /*#p11.CURE*/ wire CURE_ADDR_FF14n = not1(DUJA_ADDR_FF14p);


  /*#p11.CENU*/ wire CENU_NR10_CLKp = and2(BOGY_CPU_WRp, DYVA_ADDR_FF10p);
  /*#p11.CENU*/ wire CENU_NR10_CLKn = not1(CENU_NR10_CLKp);
  /*#p09.AGUR*/ wire AGUR_RESETn = not1(KEBA_APU_RSTp);

  /*#p11.BANY*/ BANY_NR10_SHIFT0    .dff9(CENU_NR10_CLKn, AGUR_RESETn, BUS_CPU_D00p.qp_new());
  /*#p11.ARAX*/ ARAX_NR10_SHIFT1    .dff9(CENU_NR10_CLKn, AGUR_RESETn, BUS_CPU_D01p.qp_new());
  /*#p11.ANAZ*/ ANAZ_NR10_SHIFT2    .dff9(CENU_NR10_CLKn, AGUR_RESETn, BUS_CPU_D02p.qp_new());
  /*#p11.AVAF*/ AVAF_NR10_SWEEP_DIR .dff9(CENU_NR10_CLKn, AGUR_RESETn, BUS_CPU_D03p.qp_new());
  /*#p11.ADEK*/ ADEK_NR10_DELAY0    .dff9(CENU_NR10_CLKn, AGUR_RESETn, BUS_CPU_D04p.qp_new());
  /*#p11.BANA*/ BANA_NR10_DELAY1    .dff9(CENU_NR10_CLKn, AGUR_RESETn, BUS_CPU_D05p.qp_new());
  /*#p11.BOTU*/ BOTU_NR10_DELAY2    .dff9(CENU_NR10_CLKn, AGUR_RESETn, BUS_CPU_D06p.qp_new());

  /*#p11.ATYN*/ wire ATYN_NR10_RDp = nor2(AGUZ_CPU_RDn, BUZE_ADDR_FF10n);
  /*#p11.ASOP*/ wire ASOP_NR10_RDn  = not1(ATYN_NR10_RDp);

  // Isn't the data polarity here backwards?
  /*#p11.AMYD*/ triwire AMYD_NR10_SHIFT0    = tri6_nn(ASOP_NR10_RDn, BANY_NR10_SHIFT0.qp_new());
  /*#p11.ATAX*/ triwire ATAX_NR10_SHIFT1    = tri6_nn(ASOP_NR10_RDn, ARAX_NR10_SHIFT1.qp_new());
  /*#p11.AZYP*/ triwire AZYP_NR10_SHIFT2    = tri6_nn(ASOP_NR10_RDn, ANAZ_NR10_SHIFT2.qp_new());
  /*#p11.AFOX*/ triwire AFOX_NR10_SWEEP_DIR = tri6_nn(ASOP_NR10_RDn, AVAF_NR10_SWEEP_DIR.qp_new());
  /*#p11.AVEK*/ triwire AVEK_NR10_DELAY0    = tri6_nn(ASOP_NR10_RDn, ADEK_NR10_DELAY0.qp_new());
  /*#p11.AKUX*/ triwire AKUX_NR10_DELAY1    = tri6_nn(ASOP_NR10_RDn, BANA_NR10_DELAY1.qp_new());
  /*#p11.AWOS*/ triwire AWOS_NR10_DELAY     = tri6_nn(ASOP_NR10_RDn, BOTU_NR10_DELAY2.qp_new());

  //----------
  // FF11 NR11

  /*#p11.CAMY*/ wire CAMY_NR14_RSTa = not1(KEBA_APU_RSTp);
  /*#p13.DADO*/ wire DADO_NR14_RSTb = nor2(KEBA_APU_RSTp, EZEC_START.qp_new());

  /*#p11.BAGE*/ wire BAGE_NR14_CLKa = nand2(ANUJ_CPU_WR_WEIRD, DUJA_ADDR_FF14p);
  /*#p11.BOKO*/ BOKO_NR14_STOP.dff9(BAGE_NR14_CLKa, CAMY_NR14_RSTa, BUS_CPU_D06p.qp_new());

  /*#p13.DOGE*/ wire DOGE_FF14_WRn = nand2(BOGY_CPU_WRp, DUJA_ADDR_FF14p);
  /*#p13.DUPE*/ DUPE_NR14_START.dff9(DOGE_FF14_WRn, DADO_NR14_RSTb, BUS_CPU_D07p.qp_new());

  /*#p11.BUDA*/ wire BUDA_CPU_RDo = not1(AGUZ_CPU_RDn);
  /*#p11.BALE*/ wire BALE_NR14_RD = nand2(DUJA_ADDR_FF14p, BUDA_CPU_RDo);

  /*#p11.BYTU*/ triwire BYTU = tri6_nn(BALE_NR14_RD, BOKO_NR14_STOP.qp_new());


  /*#p11.CEPO*/ wire CEPO_RESET1n = not1(KEBA_APU_RSTp);
  /*#p10.DUNO*/ wire DUNO_ADDR_0001n = nand4(ACOL_ADDR_0xxx, ABUB_ADDR_x0xx, AFOB_ADDR_xx0x, DOSO_ADDR_xxx1);
  /*#p10.CAXE*/ wire CAXE_ADDR_FF11p = nor2 (BANU_ADDR_FF1Xn, DUNO_ADDR_0001n);

  /*#p11.BUWA*/ wire BUWA_CPU_RDp = not1(AGUZ_CPU_RDn);
  /*#p11.BEXU*/ wire BEXU_NR11_RDn = nand2(BUWA_CPU_RDp, CAXE_ADDR_FF11p);
  /*#p11.COVU*/ wire COVU_NR11_WR = and2(BOGY_CPU_WRp, CAXE_ADDR_FF11p);

  /*#p11.DAFO*/ wire DAFO_NR11_CLK = not1(COVU_NR11_WR);
  /*#p11.CENA*/ CENA_NR11_DUTY_0.dff9(DAFO_NR11_CLK, CEPO_RESET1n, BUS_CPU_D06p.qp_new());
  /*#p11.DYCA*/ DYCA_NR11_DUTY_1.dff9(DAFO_NR11_CLK, CEPO_RESET1n, BUS_CPU_D07p.qp_new());

  /*#p13.CAPY*/ wire CAPY_CH1_LEN_CLKn = nor3(CERO_CH1_LEN_DONE.qp_new(), BUFY_CLK_256, BOKO_NR14_STOP.qp_new());
  /*#p13.CANU*/ wire CANU_CH1_LEN_CLK = not1(CAPY_CH1_LEN_CLKn);

  /*#p13.BORO*/ wire BORO_FF11_WRna = nand2(BOGY_CPU_WRp, CAXE_ADDR_FF11p);
  /*#p13.BEPE*/ wire BEPE_FF11_WRa = not1(BORO_FF11_WRna);
  /*#p13.BUGY*/ wire BUGY_FF11_WRc = not1(BORO_FF11_WRna);
  /*#p13.BOKA*/ wire BOKA_FF11_WRb = not1(BORO_FF11_WRna);

  /*#p13.CORY*/ wire CORY_CH1_LEN_DONE_RST = nor3(BOKA_FF11_WRb, KEBA_APU_RSTp, FEKU_RESTART0.qp_new());
  
  /*_p13.CERO*/ CERO_CH1_LEN_DONE.dff17(ERAM_NR11_LEN5.qn_new(), CORY_CH1_LEN_DONE_RST, CERO_CH1_LEN_DONE.qn_old());

  /*#p13.BACY*/ BACY_NR11_LEN0.dff20(CANU_CH1_LEN_CLK,          BUGY_FF11_WRc, BUS_CPU_D00p.qp_new());
  /*#p13.CAVY*/ CAVY_NR11_LEN1.dff20(BACY_NR11_LEN0.qp_new(),   BUGY_FF11_WRc, BUS_CPU_D01p.qp_new());
  /*#p13.BOVY*/ BOVY_NR11_LEN2.dff20(CAVY_NR11_LEN1.qp_new(),   BUGY_FF11_WRc, BUS_CPU_D02p.qp_new());
  /*#p13.CUNO*/ CUNO_NR11_LEN3.dff20(BOVY_NR11_LEN2.qp_new(),   BUGY_FF11_WRc, BUS_CPU_D03p.qp_new());

  /*#p13.CUSO*/ wire CUSO_NR11_LEN3p = not1(CUNO_NR11_LEN3.qn_new());
  /*#p13.CURA*/ CURA_NR11_LEN4.dff20(CUSO_NR11_LEN3p,           BEPE_FF11_WRa, BUS_CPU_D04p.qp_new());
  /*#p13.ERAM*/ ERAM_NR11_LEN5.dff20(CURA_NR11_LEN4.qp_new(),   BEPE_FF11_WRa, BUS_CPU_D05p.qp_new());

  /*#p11.BOWO*/ triwire BOWO = tri6_nn(BEXU_NR11_RDn, CENA_NR11_DUTY_0.qp_new());
  /*#p11.CUDA*/ triwire CUDA = tri6_nn(BEXU_NR11_RDn, DYCA_NR11_DUTY_1.qp_new());

  //----------
  // FF12 NR12

  /*#p11.HATO*/ wire HATO_RSTn = not1(KEBA_APU_RSTp);
  /*#p11.HAFU*/ wire HAFU_NR12_WRp = and2(BOGY_CPU_WRp, EDAF_ADDR_FF12p);
  /*#p11.KYGY*/ wire KYGY_NR12_WRn = not1(HAFU_NR12_WRp);
  /*#p11.GAXU*/ wire GAXU_NR12_WRn = nand2(EDAF_ADDR_FF12p, BOGY_CPU_WRp);

  // not on schematic?
  wire KAGY_NR12_WRp = not1(GAXU_NR12_WRn);

  /*#p11.JUSA*/ JUSA_NR12_DELAY0  .dff9(KYGY_NR12_WRn, HATO_RSTn, BUS_CPU_D00p.qp_new());
  /*#p11.JUZY*/ JUZY_NR12_DELAY1  .dff9(KYGY_NR12_WRn, HATO_RSTn, BUS_CPU_D01p.qp_new());
  /*#p11.JOMA*/ JOMA_NR12_DELAY2  .dff9(KYGY_NR12_WRn, HATO_RSTn, BUS_CPU_D02p.qp_new());

  /*#p11.JAFY*/ JAFY_NR12_ENV_DIR .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D03p.qp_new());
  /*#p11.JATY*/ JATY_NR12_VOL0    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D04p.qp_new());

  /*#p11.JAXO*/ JAXO_NR12_VOL1    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D05p.qp_new());
  /*#p11.JENA*/ JENA_NR12_VOL2    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D06p.qp_new());
  /*#p11.JOPU*/ JOPU_NR12_VOL3    .dff9(KAGY_NR12_WRp, HATO_RSTn, BUS_CPU_D07p.qp_new());

  /*#p11.HAXE*/ wire HAXE_ADDR_FF12n = not1(EDAF_ADDR_FF12p);
  /*#p11.GAGO*/ wire GAGO_ADDR_FF12n = not1(EDAF_ADDR_FF12p);
  /*#p11.HAMY*/ wire HAMY_NR12_RDn = or2(HAXE_ADDR_FF12n, AGUZ_CPU_RDn);
  /*#p11.HOCU*/ wire HOCU_NR12_RDn = or2(GAGO_ADDR_FF12n, AGUZ_CPU_RDn);

  // data polarity backwards again?
  /*#p11.JYNE*/ triwire JYNE = tri6_nn(HAMY_NR12_RDn, JUSA_NR12_DELAY0.qp_new());
  /*#p11.JACA*/ triwire JACA = tri6_nn(HAMY_NR12_RDn, JUZY_NR12_DELAY1.qp_new());
  /*#p11.JOKU*/ triwire JOKU = tri6_nn(HAMY_NR12_RDn, JOMA_NR12_DELAY2.qp_new());
  /*#p11.HONO*/ triwire HONO = tri6_nn(HOCU_NR12_RDn, JAFY_NR12_ENV_DIR.qp_new());
  /*#p11.HOWU*/ triwire HOWU = tri6_nn(HOCU_NR12_RDn, JATY_NR12_VOL0.qp_new());
  /*#p11.HEWA*/ triwire HEWA = tri6_nn(HOCU_NR12_RDn, JAXO_NR12_VOL1.qp_new());
  /*#p11.HEVE*/ triwire HEVE = tri6_nn(HOCU_NR12_RDn, JENA_NR12_VOL2.qp_new());
  /*#p11.JYSE*/ triwire JYSE = tri6_nn(HOCU_NR12_RDn, JOPU_NR12_VOL3.qp_new());

  /*#p13.HOCA*/ wire HOCA_CH1_AMP_ENn = nor5(JAFY_NR12_ENV_DIR.qn_new(),
                                             JATY_NR12_VOL0.qn_new(),
                                             JAXO_NR12_VOL1.qn_new(),
                                             JENA_NR12_VOL2.qn_new(),
                                             JOPU_NR12_VOL3.qn_new());

  //----------
  // Frequency can be read from FF13/FF14 in debug mode

  /*#p11.CEGE*/ wire CEGE_CPU_RDp  = not1(AGUZ_CPU_RDn);
  /*#p11.DAXA*/ wire DAXA_CPU_RDn = nand2(CEGE_CPU_RDp, EDEK_NR52_DBG_APUn);
  /*#p11.DYPU*/ wire DYPU_FF13_RDn = nor2(CACA_ADDR_FF13n, DAXA_CPU_RDn);
  /*#p11.EVAJ*/ wire EVAJ_FF13_RDa = not1(DYPU_FF13_RDn);
  /*#p11.DUPY*/ wire DUPY_FF14_RDa = or2(CURE_ADDR_FF14n, DAXA_CPU_RDn);

  /*_p11.FORU*/ triwire FORU = tri6_nn(EVAJ_FF13_RDa, GAXE_FREQ_CNT_00.qn_new());
  /*_p11.GEFU*/ triwire GEFU = tri6_nn(EVAJ_FF13_RDa, HYFE_FREQ_CNT_01.qn_new());
  /*_p11.KYVU*/ triwire KYVU = tri6_nn(EVAJ_FF13_RDa, JYTY_FREQ_CNT_02.qn_new());
  /*_p11.KUMO*/ triwire KUMO = tri6_nn(EVAJ_FF13_RDa, KYNA_FREQ_CNT_03.qn_new());
  /*_p11.KARY*/ triwire KARY = tri6_nn(EVAJ_FF13_RDa, JEMA_FREQ_CNT_04.qn_new());
  /*_p11.GODE*/ triwire GODE = tri6_nn(EVAJ_FF13_RDa, HYKE_FREQ_CNT_05.qn_new());
  /*_p11.GOJE*/ triwire GOJE = tri6_nn(EVAJ_FF13_RDa, FEVA_FREQ_CNT_06.qn_new());
  /*_p11.FOZE*/ triwire FOZE = tri6_nn(EVAJ_FF13_RDa, EKOV_FREQ_CNT_07.qn_new());
  /*_p11.DOPA*/ triwire DOPA = tri6_nn(DUPY_FF14_RDa, EMUS_FREQ_CNT_08.qn_new());
  /*_p11.DEMU*/ triwire DEMU = tri6_nn(DUPY_FF14_RDa, EVAK_FREQ_CNT_09.qn_new());
  /*_p11.DEXO*/ triwire DEXO = tri6_nn(DUPY_FF14_RDa, COPU_FREQ_CNT_10.qn_new());

  //----------
  // Sweep timer

  /*#p09.ATYV*/ wire ATYV_RESETo = not1(KEBA_APU_RSTp);
  /*#p09.BAZA*/ BAZA_DBG_SWEEP_CLK.dff17(spu.AJER_2M.qn_new(), ATYV_RESETo, BOWY_NR52_DBG_SWEEP.qp_old());

  /*#p09.CELY*/ wire CELY = mux2p(EDEK_NR52_DBG_APUn, BAZA_DBG_SWEEP_CLK.qp_new(), BYFE_CLK_128);
  /*#p09.CONE*/ wire CONE = not1(CELY);
  /*#p09.CATE*/ wire CATE_SWEEP_DELAY_CLK = not1(CONE);
  /*#p13.DAFA*/ wire DAFA = nor2(BEXA_SWEEP_TRIGGER.qp_new(), FEKU_RESTART0.qp_new());
  /*#p13.CYMU*/ wire CYMU_SWEEP_DELAY_RSTn = not1(DAFA);

  /*#p13.CUPO*/ CUPO_SWEEP_DELAY0.dff20(CATE_SWEEP_DELAY_CLK,       CYMU_SWEEP_DELAY_RSTn, ADEK_NR10_DELAY0.qp_old());
  /*#p13.CYPU*/ CYPU_SWEEP_DELAY1.dff20(CUPO_SWEEP_DELAY0.qp_new(), CYMU_SWEEP_DELAY_RSTn, BANA_NR10_DELAY1.qp_old());
  /*#p13.CAXY*/ CAXY_SWEEP_DELAY2.dff20(CYPU_SWEEP_DELAY1.qp_new(), CYMU_SWEEP_DELAY_RSTn, BOTU_NR10_DELAY2.qp_old());

  /*#p13.BAVE*/ wire BAVE_NR10_NO_SWEEP  = and3(BOTU_NR10_DELAY2.qp_new(), BANA_NR10_DELAY1.qp_new(), ADEK_NR10_DELAY0.qp_new());
  /*#p13.BURY*/ wire BURY_SWEEP_TRIGGER_RSTn = nor2(BAVE_NR10_NO_SWEEP, KEBA_APU_RSTp);
  /*#p13.COZE*/ wire COZE_SWEEP_MAX = and3(CAXY_SWEEP_DELAY2.qp_new(), CYPU_SWEEP_DELAY1.qp_new(), CUPO_SWEEP_DELAY0.qp_new());

  /*#p13.BEXA*/ BEXA_SWEEP_TRIGGER.dff17(spu.AJER_2M.qp_new(), BURY_SWEEP_TRIGGER_RSTn, COZE_SWEEP_MAX);

  //----------
  // Frequency summer

  /*#p13.KABA*/ wire KABA_SUM_RST = or2(KEBA_APU_RSTp, FEKU_RESTART0.qp_new());
  /*#p13.KYNY*/ wire KYNY_SUM_RSTn = not1(KABA_SUM_RST);
  /*#p12.ARYL*/ wire ARYL_NR10_SWEEP_DIRn = not1(AVAF_NR10_SWEEP_DIR.qp_new());

  /*_p12.GYLO*/ wire GYLO_SHIFTER_FLIP_00 = xor2(ARYL_NR10_SWEEP_DIRn, FABU_SHIFTER_00.qp_new());
  /*_p12.GELA*/ wire GELA_SHIFTER_FLIP_01 = xor2(ARYL_NR10_SWEEP_DIRn, JEFA_SHIFTER_01.qp_new());
  /*_p12.GOPO*/ wire GOPO_SHIFTER_FLIP_02 = xor2(ARYL_NR10_SWEEP_DIRn, GOGA_SHIFTER_02.qp_new());
  /*_p12.HEFY*/ wire HEFY_SHIFTER_FLIP_03 = xor2(ARYL_NR10_SWEEP_DIRn, JOLU_SHIFTER_03.qp_new());
  /*_p12.KEFE*/ wire KEFE_SHIFTER_FLIP_04 = xor2(ARYL_NR10_SWEEP_DIRn, JOTA_SHIFTER_04.qp_new());
  /*_p12.GOLY*/ wire GOLY_SHIFTER_FLIP_05 = xor2(ARYL_NR10_SWEEP_DIRn, FUDE_SHIFTER_05.qp_new());
  /*_p12.FURE*/ wire FURE_SHIFTER_FLIP_06 = xor2(ARYL_NR10_SWEEP_DIRn, FEDO_SHIFTER_06.qp_new());
  /*_p12.DYME*/ wire DYME_SHIFTER_FLIP_07 = xor2(ARYL_NR10_SWEEP_DIRn, EXAC_SHIFTER_07.qp_new());
  /*_p12.CALE*/ wire CALE_SHIFTER_FLIP_08 = xor2(ARYL_NR10_SWEEP_DIRn, ELUX_SHIFTER_08.qp_new());
  /*_p12.DOZY*/ wire DOZY_SHIFTER_FLIP_09 = xor2(ARYL_NR10_SWEEP_DIRn, AGEZ_SHIFTER_09.qp_new());
  /*_p12.CULU*/ wire CULU_SHIFTER_FLIP_10 = xor2(ARYL_NR10_SWEEP_DIRn, BEKU_SHIFTER_10.qp_new());

  /*_p13.ADAD*/ wire ADAD_SHIFT_DONE_SYNCn = not1(BYTE_SHIFT_DONE_SYNC.qn_new());

  /*_p12.GALO*/ GALO_SUM_A00.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, HYKA_SUMMER00.qp_old());
  /*_p12.JODE*/ JODE_SUM_A01.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, JYKA_SUMMER01.qp_old());
  /*_p12.KARE*/ KARE_SUM_A02.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, HAVO_SUMMER02.qp_old());
  /*_p12.JYME*/ JYME_SUM_A03.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, EDUL_SUMMER03.qp_old());
  /*_p12.GYME*/ GYME_SUM_A04.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, FELY_SUMMER04.qp_old());
  /*_p12.FAXO*/ FAXO_SUM_A05.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, HOLU_SUMMER05.qp_old());
  /*_p12.EXAP*/ EXAP_SUM_A06.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, HYXU_SUMMER06.qp_old());
  /*_p12.DELE*/ DELE_SUM_A07.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, HOPO_SUMMER07.qp_old());
  /*_p12.DEXE*/ DEXE_SUM_A08.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, DYGY_SUMMER08.qp_old());
  /*_p12.DOFY*/ DOFY_SUM_A09.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, EVAB_SUMMER09.qp_old());
  /*_p12.DOLY*/ DOLY_SUM_A10.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, AXAN_SUMMER10.qp_old());

  /*_p12.HORA*/ HORA_SUM_B00.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, GYLO_SHIFTER_FLIP_00);
  /*_p12.HOPA*/ HOPA_SUM_B01.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, GELA_SHIFTER_FLIP_01);
  /*_p12.HELE*/ HELE_SUM_B02.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, GOPO_SHIFTER_FLIP_02);
  /*_p12.JAPE*/ JAPE_SUM_B03.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, HEFY_SHIFTER_FLIP_03);
  /*_p12.JETE*/ JETE_SUM_B04.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, KEFE_SHIFTER_FLIP_04);
  /*_p12.GELE*/ GELE_SUM_B05.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, GOLY_SHIFTER_FLIP_05);
  /*_p12.EPYR*/ EPYR_SUM_B06.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, FURE_SHIFTER_FLIP_06);
  /*_p12.EDOK*/ EDOK_SUM_B07.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, DYME_SHIFTER_FLIP_07);
  /*_p12.DEFA*/ DEFA_SUM_B08.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, CALE_SHIFTER_FLIP_08);
  /*_p12.ETER*/ ETER_SUM_B09.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, DOZY_SHIFTER_FLIP_09);
  /*_p12.DEVA*/ DEVA_SUM_B10.dff11(ADAD_SHIFT_DONE_SYNCn, KYNY_SUM_RSTn, CULU_SHIFTER_FLIP_10);

  /*#p12.GUXA*/ Adder GUXA_SUM00 = add3(GALO_SUM_A00.qp_new(), HORA_SUM_B00.qp_new(), ARYL_NR10_SWEEP_DIRn);
  /*_p12.HALU*/ Adder HALU_SUM01 = add3(JODE_SUM_A01.qp_new(), HOPA_SUM_B01.qp_new(), GUXA_SUM00.carry);
  /*_p12.JULE*/ Adder JULE_SUM02 = add3(KARE_SUM_A02.qp_new(), HELE_SUM_B02.qp_new(), HALU_SUM01.carry);
  /*_p12.JORY*/ Adder JORY_SUM03 = add3(JYME_SUM_A03.qp_new(), JAPE_SUM_B03.qp_new(), JULE_SUM02.carry);
  /*_p12.HEXO*/ Adder HEXO_SUM04 = add3(GYME_SUM_A04.qp_new(), JETE_SUM_B04.qp_new(), JORY_SUM03.carry);
  /*_p12.GEVA*/ Adder GEVA_SUM05 = add3(FAXO_SUM_A05.qp_new(), GELE_SUM_B05.qp_new(), HEXO_SUM04.carry);
  /*#p12.FEGO*/ Adder FEGO_SUM06 = add3(EXAP_SUM_A06.qp_new(), EPYR_SUM_B06.qp_new(), GEVA_SUM05.carry);
  /*_p12.ETEK*/ Adder ETEK_SUM07 = add3(DELE_SUM_A07.qp_new(), EDOK_SUM_B07.qp_new(), FEGO_SUM06.carry);
  /*_p12.DYXE*/ Adder DYXE_SUM08 = add3(DEXE_SUM_A08.qp_new(), DEFA_SUM_B08.qp_new(), ETEK_SUM07.carry);
  /*_p12.DULE*/ Adder DULE_SUM09 = add3(DOFY_SUM_A09.qp_new(), ETER_SUM_B09.qp_new(), DYXE_SUM08.carry);
  /*_p12.CORU*/ Adder CORU_SUM10 = add3(DOLY_SUM_A10.qp_new(), DEVA_SUM_B10.qp_new(), DULE_SUM09.carry);

  /*#p12.BYLE*/ wire BYLE = nor2(ARYL_NR10_SWEEP_DIRn, CORU_SUM10.carry);
  /*#p12.ATYS*/ wire ATYS_FREQ_OVERFLOW = or2(BYLE, ARYL_NR10_SWEEP_DIRn);

  /*#p12.DEBY*/ wire DEBY_FREQ_WRp = and2(BOGY_CPU_WRp, DUJA_ADDR_FF14p);
  /*#p12.DEPU*/ wire DEPU = nand2(BOGY_CPU_WRp, DECO_ADDR_FF13p);
  /*#p12.DYLA*/ wire DYLA_FREQ_WRc = not1(DEPU);

  /*#p12.GYLU*/ wire GYLU_SUMMER00_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D00p.qp_new());
  /*#p12.GETA*/ wire GETA_SUMMER01_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D01p.qp_new());
  /*#p12.GOLO*/ wire GOLO_SUMMER02_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D02p.qp_new());
  /*#p12.GOPE*/ wire GOPE_SUMMER03_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D03p.qp_new());
  /*#p12.KOVU*/ wire KOVU_SUMMER04_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D04p.qp_new());
  /*#p12.KYPA*/ wire KYPA_SUMMER05_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D05p.qp_new());
  /*#p12.ELER*/ wire ELER_SUMMER06_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D06p.qp_new());
  /*#p12.ETOL*/ wire ETOL_SUMMER07_SET = nand2(DYLA_FREQ_WRc, BUS_CPU_D07p.qp_new());
  /*#p12.BUGU*/ wire BUGU_SUMMER08_SET = nand2(DEBY_FREQ_WRp, BUS_CPU_D00p.qp_new());
  /*#p12.BUDO*/ wire BUDO_SUMMER09_SET = nand2(DEBY_FREQ_WRp, BUS_CPU_D01p.qp_new());
  /*#p12.AFEG*/ wire AFEG_SUMMER10_SET = nand2(DEBY_FREQ_WRp, BUS_CPU_D02p.qp_new());

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

  /*#p12.EJYF*/ wire EJYF = and2(DYLA_FREQ_WRc, DEKE);
  /*#p12.FOPU*/ wire FOPU = and2(DYLA_FREQ_WRc, GULU);
  /*#p12.FOKE*/ wire FOKE = and2(DYLA_FREQ_WRc, FULE);
  /*#p12.FEGA*/ wire FEGA = and2(DYLA_FREQ_WRc, ETUV);
  /*#p12.KAVO*/ wire KAVO = and2(DYLA_FREQ_WRc, KOPU);
  /*#p12.KYFU*/ wire KYFU = and2(DYLA_FREQ_WRc, JULO);
  /*#p12.ETOK*/ wire ETOK = and2(DYLA_FREQ_WRc, DYLU);
  /*#p12.EMAR*/ wire EMAR = and2(DYLA_FREQ_WRc, DULO);
  /*#p12.BASO*/ wire BASO = and2(DEBY_FREQ_WRp, BYSU);
  /*#p12.AMAC*/ wire AMAC = and2(DEBY_FREQ_WRp, BOFU);
  /*#p12.AJUX*/ wire AJUX = and2(DEBY_FREQ_WRp, BYFU);

  /*#p12.EFOR*/ wire EFOR_SUMMER00_RST = nor2(EJYF, KEBA_APU_RSTp); 
  /*#p12.GATO*/ wire GATO_SUMMER01_RST = nor2(FOPU, KEBA_APU_RSTp); 
  /*#p12.GYFU*/ wire GYFU_SUMMER02_RST = nor2(FOKE, KEBA_APU_RSTp); 
  /*#p12.GAMO*/ wire GAMO_SUMMER03_RST = nor2(FEGA, KEBA_APU_RSTp); 
  /*#p12.KAPO*/ wire KAPO_SUMMER04_RST = nor2(KAVO, KEBA_APU_RSTp); 
  /*#p12.KAJU*/ wire KAJU_SUMMER05_RST = nor2(KYFU, KEBA_APU_RSTp); 
  /*#p12.ELUF*/ wire ELUF_SUMMER06_RST = nor2(ETOK, KEBA_APU_RSTp); 
  /*#p12.ESEL*/ wire ESEL_SUMMER07_RST = nor2(EMAR, KEBA_APU_RSTp); 
  /*#p12.BOXU*/ wire BOXU_SUMMER08_RST = nor2(BASO, KEBA_APU_RSTp); 
  /*#p12.BOVU*/ wire BOVU_SUMMER09_RST = nor2(AMAC, KEBA_APU_RSTp); 
  /*#p12.APAJ*/ wire APAJ_SUMMER10_RST = nor2(AJUX, KEBA_APU_RSTp); 

  /*#p13.ATUV*/ wire ATUV = and2(BEXA_SWEEP_TRIGGER.qp_new(), ATYS_FREQ_OVERFLOW);
  /*#p13.BUGE*/ wire BUGE_SHIFTER_EN = nand3(ANAZ_NR10_SHIFT2.qp_new(), ARAX_NR10_SHIFT1.qp_new(), BANY_NR10_SHIFT0.qp_new());
  /*#p13.BOJE*/ wire BOJE_SUMMER_CLKa = and2(ATUV, BUGE_SHIFTER_EN);
  /*#p13.BUSO*/ wire BUSO_SUMMER_CLKb = and3(BEXA_SWEEP_TRIGGER.qp_new(), ATYS_FREQ_OVERFLOW, BUGE_SHIFTER_EN);

  /*_p12.HYKA*/ HYKA_SUMMER00.dff22(BOJE_SUMMER_CLKa, GYLU_SUMMER00_SET, EFOR_SUMMER00_RST, GUXA_SUM00.sum);
  /*_p12.JYKA*/ JYKA_SUMMER01.dff22(BOJE_SUMMER_CLKa, GETA_SUMMER01_SET, GATO_SUMMER01_RST, HALU_SUM01.sum);
  /*_p12.HAVO*/ HAVO_SUMMER02.dff22(BOJE_SUMMER_CLKa, GOLO_SUMMER02_SET, GYFU_SUMMER02_RST, JULE_SUM02.sum);
  /*_p12.EDUL*/ EDUL_SUMMER03.dff22(BOJE_SUMMER_CLKa, GOPE_SUMMER03_SET, GAMO_SUMMER03_RST, JORY_SUM03.sum);
  /*_p12.FELY*/ FELY_SUMMER04.dff22(BOJE_SUMMER_CLKa, KOVU_SUMMER04_SET, KAPO_SUMMER04_RST, HEXO_SUM04.sum);
  /*_p12.HOLU*/ HOLU_SUMMER05.dff22(BOJE_SUMMER_CLKa, KYPA_SUMMER05_SET, KAJU_SUMMER05_RST, GEVA_SUM05.sum);
  /*_p12.HYXU*/ HYXU_SUMMER06.dff22(BOJE_SUMMER_CLKa, ELER_SUMMER06_SET, ELUF_SUMMER06_RST, FEGO_SUM06.sum);
  /*_p12.HOPO*/ HOPO_SUMMER07.dff22(BOJE_SUMMER_CLKa, ETOL_SUMMER07_SET, ESEL_SUMMER07_RST, ETEK_SUM07.sum);
  /*_p12.DYGY*/ DYGY_SUMMER08.dff22(BUSO_SUMMER_CLKb, BUGU_SUMMER08_SET, BOXU_SUMMER08_RST, DYXE_SUM08.sum);
  /*_p12.EVAB*/ EVAB_SUMMER09.dff22(BUSO_SUMMER_CLKb, BUDO_SUMMER09_SET, BOVU_SUMMER09_RST, DULE_SUM09.sum);
  /*_p12.AXAN*/ AXAN_SUMMER10.dff22(BUSO_SUMMER_CLKb, AFEG_SUMMER10_SET, APAJ_SUMMER10_RST, CORU_SUM10.sum);

  //----------
  // Frequency shifter


  /*#p13.EPUK*/ wire EPUK = nor2(KEBA_APU_RSTp, ADAD_SHIFT_DONE_SYNCn);
  /*#p13.EVOL*/ wire EVOL = nor2(BEXA_SWEEP_TRIGGER.qp_new(), FYTE_RESTART2.qp_new());
  
  /*#p13.FEMU*/ FEMU.nand_latch(EPUK, EVOL);
  
  
  /*#p09.DYFA*/ wire DYFA_RESTART_CLKn = not1(CALO_RESTART_CLK.qn_new());
  /*#p13.EGYP*/ wire EGYP = nor2(FEMU.qn_new(), DYFA_RESTART_CLKn);
  /*#p13.CELE*/ wire CELE = not1(BUGE_SHIFTER_EN);
  /*#p13.DODY*/ wire DODY = nor2(EGYP, CELE); // border color wrong on die

  /*#p13.EGOR*/ wire EGOR_SHIFTER_CLKn = nor3(DODY, FYLO.qp_new(), BURE_CLK_512); // this looks like a nor3? black border on die
  /*#p12.FAJA*/ wire FAJA = not1(EGOR_SHIFTER_CLKn);
  /*#p12.EJYB*/ wire EJYB_SHIFTER_CLKo = not1(FAJA);
  /*#p12.CYBE*/ wire CYBE = not1(EJYB_SHIFTER_CLKo);
  /*#p12.BECY*/ wire BECY_SHIFTER_CLKp = not1(CYBE);

  /*#p13.DACU*/ wire DACU_SHIFTER_CNT_LDn = nor2(FEKU_RESTART0.qp_new(), BEXA_SWEEP_TRIGGER.qp_new());

  /*#p13.DAPU*/ wire DAPU_SHIFTER_CNT_CLK = not1(EGOR_SHIFTER_CLKn);
  /*#p13.CYLU*/ wire CYLU_SHIFTER_CNT_LD = not1(DACU_SHIFTER_CNT_LDn);

  /*#p13.COPA*/ COPA_SHIFTER_CNT0.dff20(DAPU_SHIFTER_CNT_CLK,       CYLU_SHIFTER_CNT_LD, BANY_NR10_SHIFT0.qp_new());
  /*#p13.CAJA*/ CAJA_SHIFTER_CNT1.dff20(COPA_SHIFTER_CNT0.qp_new(), CYLU_SHIFTER_CNT_LD, ARAX_NR10_SHIFT1.qp_new());
  /*#p13.BYRA*/ BYRA_SHIFTER_CNT2.dff20(CAJA_SHIFTER_CNT1.qp_new(), CYLU_SHIFTER_CNT_LD, ANAZ_NR10_SHIFT2.qp_new());

  /*#p13.ATAT*/ wire ATAT_SHIFT_DONE_SYNC_RST = nor2(KEBA_APU_RSTp, BEXA_SWEEP_TRIGGER.qp_new());
  /*#p13.COPY*/ wire COPY = and3(BYRA_SHIFTER_CNT2.qp_new(), CAJA_SHIFTER_CNT1.qp_new(), COPA_SHIFTER_CNT0.qp_new());
  /*#p13.BYTE*/ BYTE_SHIFT_DONE_SYNC.dff17(spu.AJER_2M.qp_new(), ATAT_SHIFT_DONE_SYNC_RST, COPY);

  /*#p13.KALA*/ wire KALA_SHIFTER_LOAD  = nor2(BEXA_SWEEP_TRIGGER.qp_new(), FEKU_RESTART0.qp_new());
  /*#p12.KAPE*/ wire KAPE_SHIFTER_LOADn = not1(KALA_SHIFTER_LOAD);
  /*#p12.JUJU*/ wire JUJU_SHIFTER_LOADo = not1(KALA_SHIFTER_LOAD);
  /*#p12.KEDO*/ wire KEDO_SHIFTER_LOADp = not1(KALA_SHIFTER_LOAD);

  /*_p12.JUTA*/ wire JUTA_SUMMER00n = not1(HYKA_SUMMER00.qp_new());
  /*_p12.HOXE*/ wire HOXE_SUMMER01n = not1(JYKA_SUMMER01.qp_new());
  /*_p12.HUNY*/ wire HUNY_SUMMER02n = not1(HAVO_SUMMER02.qp_new());
  /*_p12.KEKE*/ wire KEKE_SUMMER03n = not1(EDUL_SUMMER03.qp_new());
  /*_p12.KOVO*/ wire KOVO_SUMMER04n = not1(FELY_SUMMER04.qp_new());
  /*_p12.FOHY*/ wire FOHY_SUMMER05n = not1(HOLU_SUMMER05.qp_new());
  /*_p12.DEBO*/ wire DEBO_SUMMER06n = not1(HYXU_SUMMER06.qp_new());
  /*_p12.CYKY*/ wire CYKY_SUMMER07n = not1(HOPO_SUMMER07.qp_new());
  /*_p12.BYRU*/ wire BYRU_SUMMER08n = not1(DYGY_SUMMER08.qp_new());
  /*_p12.APAT*/ wire APAT_SUMMER09n = not1(EVAB_SUMMER09.qp_new());
  /*_p12.BOJO*/ wire BOJO_SUMMER10n = not1(AXAN_SUMMER10.qp_new());

  /*_p12.KOKO*/ wire KOKO = and2(JUTA_SUMMER00n, KAPE_SHIFTER_LOADn);
  /*_p12.JOCY*/ wire JOCY = and2(HOXE_SUMMER01n, KAPE_SHIFTER_LOADn);
  /*_p12.JEHY*/ wire JEHY = and2(HUNY_SUMMER02n, KAPE_SHIFTER_LOADn);
  /*_p12.KAXY*/ wire KAXY = and2(KEKE_SUMMER03n, JUJU_SHIFTER_LOADo);
  /*_p12.KEVY*/ wire KEVY = and2(KOVO_SUMMER04n, JUJU_SHIFTER_LOADo);
  /*_p12.EVOF*/ wire EVOF = and2(FOHY_SUMMER05n, JUJU_SHIFTER_LOADo);
  /*_p12.EREG*/ wire EREG = and2(DEBO_SUMMER06n, JUJU_SHIFTER_LOADo);
  /*_p12.BAPU*/ wire BAPU = and2(CYKY_SUMMER07n, KEDO_SHIFTER_LOADp);
  /*_p12.AFUG*/ wire AFUG = and2(BYRU_SUMMER08n, KEDO_SHIFTER_LOADp);
  /*_p12.BUVO*/ wire BUVO = and2(APAT_SUMMER09n, KEDO_SHIFTER_LOADp);
  /*_p12.AFYR*/ wire AFYR = and2(BOJO_SUMMER10n, KEDO_SHIFTER_LOADp);

  /*_p12.HOZU*/ wire HOZU_SHIFTER_SET_00 = nand2(HYKA_SUMMER00.qp_new(), KAPE_SHIFTER_LOADn);
  /*_p12.HOLA*/ wire HOLA_SHIFTER_SET_01 = nand2(JYKA_SUMMER01.qp_new(), KAPE_SHIFTER_LOADn);
  /*_p12.HAWY*/ wire HAWY_SHIFTER_SET_02 = nand2(HAVO_SUMMER02.qp_new(), KAPE_SHIFTER_LOADn);
  /*_p12.KYRY*/ wire KYRY_SHIFTER_SET_03 = nand2(EDUL_SUMMER03.qp_new(), JUJU_SHIFTER_LOADo);
  /*_p12.KOLA*/ wire KOLA_SHIFTER_SET_04 = nand2(FELY_SUMMER04.qp_new(), JUJU_SHIFTER_LOADo);
  /*_p12.GOVO*/ wire GOVO_SHIFTER_SET_05 = nand2(HOLU_SUMMER05.qp_new(), JUJU_SHIFTER_LOADo);
  /*_p12.EKEM*/ wire EKEM_SHIFTER_SET_06 = nand2(HYXU_SUMMER06.qp_new(), JUJU_SHIFTER_LOADo);
  /*_p12.DACE*/ wire DACE_SHIFTER_SET_07 = nand2(HOPO_SUMMER07.qp_new(), KEDO_SHIFTER_LOADp);
  /*_p12.BEGE*/ wire BEGE_SHIFTER_SET_08 = nand2(DYGY_SUMMER08.qp_new(), KEDO_SHIFTER_LOADp);
  /*_p12.BESO*/ wire BESO_SHIFTER_SET_09 = nand2(EVAB_SUMMER09.qp_new(), KEDO_SHIFTER_LOADp);
  /*_p12.BEJU*/ wire BEJU_SHIFTER_SET_10 = nand2(AXAN_SUMMER10.qp_new(), KEDO_SHIFTER_LOADp);

  /*_p12.JADO*/ wire JADO_SHIFTER_RST_00 = nor2(KEBA_APU_RSTp, KOKO);
  /*_p12.HOBU*/ wire HOBU_SHIFTER_RST_01 = nor2(KEBA_APU_RSTp, JOCY);
  /*_p12.HYVU*/ wire HYVU_SHIFTER_RST_02 = nor2(KEBA_APU_RSTp, JEHY);
  /*_p12.KETO*/ wire KETO_SHIFTER_RST_03 = nor2(KEBA_APU_RSTp, KAXY);
  /*_p12.KYBO*/ wire KYBO_SHIFTER_RST_04 = nor2(KEBA_APU_RSTp, KEVY);
  /*_p12.EZUK*/ wire EZUK_SHIFTER_RST_05 = nor2(KEBA_APU_RSTp, EVOF);
  /*_p12.ENOK*/ wire ENOK_SHIFTER_RST_06 = nor2(KEBA_APU_RSTp, EREG);
  /*_p12.BEWO*/ wire BEWO_SHIFTER_RST_07 = nor2(KEBA_APU_RSTp, BAPU);
  /*_p12.AGOR*/ wire AGOR_SHIFTER_RST_08 = nor2(KEBA_APU_RSTp, AFUG);
  /*_p12.AFUX*/ wire AFUX_SHIFTER_RST_09 = nor2(KEBA_APU_RSTp, BUVO);
  /*_p12.AVUF*/ wire AVUF_SHIFTER_RST_10 = nor2(KEBA_APU_RSTp, AFYR);

  /*#p12.FABU*/ FABU_SHIFTER_00.dff22(EGOR_SHIFTER_CLKn, HOZU_SHIFTER_SET_00, JADO_SHIFTER_RST_00, JEFA_SHIFTER_01.qp_old());
  /*_p12.JEFA*/ JEFA_SHIFTER_01.dff22(EGOR_SHIFTER_CLKn, HOLA_SHIFTER_SET_01, HOBU_SHIFTER_RST_01, GOGA_SHIFTER_02.qp_old());
  /*_p12.GOGA*/ GOGA_SHIFTER_02.dff22(EGOR_SHIFTER_CLKn, HAWY_SHIFTER_SET_02, HYVU_SHIFTER_RST_02, JOLU_SHIFTER_03.qp_old());
  /*_p12.JOLU*/ JOLU_SHIFTER_03.dff22(EJYB_SHIFTER_CLKo, KYRY_SHIFTER_SET_03, KETO_SHIFTER_RST_03, JOTA_SHIFTER_04.qp_old());
  /*_p12.JOTA*/ JOTA_SHIFTER_04.dff22(EJYB_SHIFTER_CLKo, KOLA_SHIFTER_SET_04, KYBO_SHIFTER_RST_04, FUDE_SHIFTER_05.qp_old());
  /*_p12.FUDE*/ FUDE_SHIFTER_05.dff22(EJYB_SHIFTER_CLKo, GOVO_SHIFTER_SET_05, EZUK_SHIFTER_RST_05, FEDO_SHIFTER_06.qp_old());
  /*_p12.FEDO*/ FEDO_SHIFTER_06.dff22(EJYB_SHIFTER_CLKo, EKEM_SHIFTER_SET_06, ENOK_SHIFTER_RST_06, EXAC_SHIFTER_07.qp_old());
  /*_p12.EXAC*/ EXAC_SHIFTER_07.dff22(BECY_SHIFTER_CLKp, DACE_SHIFTER_SET_07, BEWO_SHIFTER_RST_07, ELUX_SHIFTER_08.qp_old());
  /*_p12.ELUX*/ ELUX_SHIFTER_08.dff22(BECY_SHIFTER_CLKp, BEGE_SHIFTER_SET_08, AGOR_SHIFTER_RST_08, AGEZ_SHIFTER_09.qp_old());
  /*_p12.AGEZ*/ AGEZ_SHIFTER_09.dff22(BECY_SHIFTER_CLKp, BESO_SHIFTER_SET_09, AFUX_SHIFTER_RST_09, BEKU_SHIFTER_10.qp_old());
  /*_p12.BEKU*/ BEKU_SHIFTER_10.dff22(BECY_SHIFTER_CLKp, BEJU_SHIFTER_SET_10, AVUF_SHIFTER_RST_10, 0);

  //----------
  // Restart sequencer

  /*#p13.DUKA*/ wire DUKA = not1(KEBA_APU_RSTp);
  /*#p13.EZEC*/ EZEC_START.dff17(DOVA_ABCDxxxx, DUKA, DUPE_NR14_START.qn_new());


  /*#p13.EGET*/ wire EGET = nor2(KEBA_APU_RSTp, FARE_RESTART1.qp_new());
  /*#p13.GEFE*/ wire GEFE_RESTART0_RSTn = not1(EGET);
  /*#p13.ERUM*/ wire ERUM_RESTART_RSTo = not1(KEBA_APU_RSTp);

  /*#p13.FYFO*/ FYFO.nor_latch(GEFE_RESTART0_RSTn, EZEC_START.qp_new());

  /*#p01.BATA*/ wire BATA_CLK_2Mn = not1(spu.AJER_2M.qp_new());
  /*#p09.CALO*/ CALO_RESTART_CLK.dff17(BATA_CLK_2Mn, AGUR_RESETn, CALO_RESTART_CLK.qn_new());

  /*#p13.FEKU*/ FEKU_RESTART0.dff17(DYFA_RESTART_CLKn, EGET,              FYFO.qn_old()); // schematic wrong?
  /*#p13.FARE*/ FARE_RESTART1.dff17(DYFA_RESTART_CLKn, ERUM_RESTART_RSTo, FEKU_RESTART0.qp_old());
  /*#p13.FYTE*/ FYTE_RESTART2.dff17(DYFA_RESTART_CLKn, ERUM_RESTART_RSTo, FARE_RESTART1.qp_old());

  //----------
  // Waveform generator

  /*#p13.CALA*/ wire CALA_PHASE_CLK_CLK = not1(COPU_FREQ_CNT_10.qp_new());
  /*#p13.DOKA*/ wire DOKA = and2(COMY_PHASE_CLKnb.qp_new(), DYFA_RESTART_CLKn);
  /*#p13.DYRU*/ wire DYRU_PHASE_CLK_RST = nor3(KEBA_APU_RSTp, FEKU_RESTART0.qp_new(), DOKA);

  /*_p13.COMY*/ COMY_PHASE_CLKnb.dff17(CALA_PHASE_CLK_CLK, DYRU_PHASE_CLK_RST, COMY_PHASE_CLKnb.qn_old());
  /*#p13.CYTE*/ wire CYTE = not1(COMY_PHASE_CLKnb.qp_new());
  /*#p13.COPE*/ wire COPE_PHASE_CLKna = not1(CYTE);

  /*#p13.FEMY*/ wire FEMY = nor2(HOCA_CH1_AMP_ENn, KEBA_APU_RSTp);
  /*#p13.GEPU*/ wire GEPU = not1(FYTE_RESTART2.qp_new());
  /*#p13.GEXU*/ GEXU_FREQ_GATE.nand_latch(FEMY, GEPU);

  /*#p11.EPYK*/ wire EPYK_FREQ_RSTn = nor2(COPE_PHASE_CLKna, FEKU_RESTART0.qp_new());
  /*#p11.FUME*/ wire FUME_FREQ_RSTa = not1(EPYK_FREQ_RSTn);
  /*#p11.DEGA*/ wire DEGA_FREQ_RSTb = not1(EPYK_FREQ_RSTn);
  /*#p11.DAKO*/ wire DAKO_FREQ_RSTc = not1(EPYK_FREQ_RSTn);

  /*#p11.FULO*/ wire FULO = nor2(DYFA_RESTART_CLKn, GEXU_FREQ_GATE.qp_new());
  /*#p11.GEKU*/ wire GEKU_FREQ_CLKa = not1(FULO);
  /*#p11.KYPE*/ wire KYPE_FREQ_CLKb = not1(KYNA_FREQ_CNT_03.qn_new());
  /*#p11.DERU*/ wire DERU_FREQ_CLKc = not1(EKOV_FREQ_CNT_07.qn_new());

  /*#p11.GAXE*/ GAXE_FREQ_CNT_00.dff20(GEKU_FREQ_CLKa,            FUME_FREQ_RSTa, HYKA_SUMMER00.qp_new());
  /*#p11.HYFE*/ HYFE_FREQ_CNT_01.dff20(GAXE_FREQ_CNT_00.qp_new(), FUME_FREQ_RSTa, JYKA_SUMMER01.qp_new());
  /*_p11.JYTY*/ JYTY_FREQ_CNT_02.dff20(HYFE_FREQ_CNT_01.qp_new(), FUME_FREQ_RSTa, HAVO_SUMMER02.qp_new());
  /*_p11.KYNA*/ KYNA_FREQ_CNT_03.dff20(JYTY_FREQ_CNT_02.qp_new(), FUME_FREQ_RSTa, EDUL_SUMMER03.qp_new());
  /*_p11.JEMA*/ JEMA_FREQ_CNT_04.dff20(KYPE_FREQ_CLKb,            DEGA_FREQ_RSTb, FELY_SUMMER04.qp_new());
  /*_p11.HYKE*/ HYKE_FREQ_CNT_05.dff20(JEMA_FREQ_CNT_04.qp_new(), DEGA_FREQ_RSTb, HOLU_SUMMER05.qp_new());
  /*_p11.FEVA*/ FEVA_FREQ_CNT_06.dff20(HYKE_FREQ_CNT_05.qp_new(), DEGA_FREQ_RSTb, HYXU_SUMMER06.qp_new());
  /*_p11.EKOV*/ EKOV_FREQ_CNT_07.dff20(FEVA_FREQ_CNT_06.qp_new(), DEGA_FREQ_RSTb, HOPO_SUMMER07.qp_new());
  /*_p11.EMUS*/ EMUS_FREQ_CNT_08.dff20(DERU_FREQ_CLKc,            DAKO_FREQ_RSTc, DYGY_SUMMER08.qp_new());
  /*_p11.EVAK*/ EVAK_FREQ_CNT_09.dff20(EVAK_FREQ_CNT_09.qp_new(), DAKO_FREQ_RSTc, EVAB_SUMMER09.qp_new());
  /*_p11.COPU*/ COPU_FREQ_CNT_10.dff20(COPU_FREQ_CNT_10.qp_new(), DAKO_FREQ_RSTc, AXAN_SUMMER10.qp_new());

  /*#p11.CEPO*/ wire RESET1n = not1(KEBA_APU_RSTp);

  /*#p13.DAJO*/ wire DAJO_PHASE_CLKa = not1(COPE_PHASE_CLKna);
  /*#p13.ESUT*/ ESUT_PHASE_xBxDxFxH.dff17(DAJO_PHASE_CLKa,              CEPO_RESET1n, ESUT_PHASE_xBxDxFxH.qn_old());
  /*#p13.EROS*/ EROS_PHASE_xxCDxxGH.dff13(ESUT_PHASE_xBxDxFxH.qn_new(), CEPO_RESET1n, EROS_PHASE_xxCDxxGH.qn_old());
  /*#p13.DAPE*/ DAPE_PHASE_xxxxEFGH.dff13(EROS_PHASE_xxCDxxGH.qn_new(), CEPO_RESET1n, DAPE_PHASE_xxxxEFGH.qn_old());

  /*#p13.DUVO*/ wire DUVO_PHASE_AxCxExGx = not1(ESUT_PHASE_xBxDxFxH.qp_new());
  /*#p13.EZOZ*/ wire EZOZ_PHASE_xxxxxxGH = and2(DAPE_PHASE_xxxxEFGH.qp_new(), EROS_PHASE_xxCDxxGH.qp_new());
  /*#p13.ENEK*/ wire ENEK_PHASE_xxxxxxGx = and2(EZOZ_PHASE_xxxxxxGH, DUVO_PHASE_AxCxExGx);
  /*#p13.CODO*/ wire CODO_PHASE_ABCDEFxx = not1(EZOZ_PHASE_xxxxxxGH);

  /*#p13.COSO*/ wire COSO = nor2(CENA_NR11_DUTY_0.qn_new(), DYCA_NR11_DUTY_1.qn_new());
  /*#p13.CAVA*/ wire CAVA = nor2(CENA_NR11_DUTY_0.qp_new(), DYCA_NR11_DUTY_1.qn_new());
  /*#p13.CEVU*/ wire CEVU = nor2(CENA_NR11_DUTY_0.qn_new(), DYCA_NR11_DUTY_1.qp_new());
  /*#p13.CAXO*/ wire CAXO = nor2(CENA_NR11_DUTY_0.qp_new(), DYCA_NR11_DUTY_1.qp_new());
  /*#p13.DUNA*/ wire DUNA_RAW_BIT = amux4(ENEK_PHASE_xxxxxxGx, COSO,
                                          EZOZ_PHASE_xxxxxxGH, CAVA,
                                          DAPE_PHASE_xxxxEFGH.qp_new(), CEVU,
                                          CODO_PHASE_ABCDEFxx, CAXO);
  /*_p13.DUWO*/ DUWO_RAW_BIT_SYNC.dff17(COPE_PHASE_CLKna, CEPO_RESET1n, DUNA_RAW_BIT);

  //----------
  // Envelope generator. I'm not entirely sure how this works...

  /*#p13.KAZA*/ wire KAZA = nor2(FEKU_RESTART0.qp_new(), KOZY_ENV_CLK.qp_new());
  /*#p13.KUXU*/ wire KUXU_ENV_DELAY_LOAD = not1(KAZA);

  /*#p13.JONE*/ wire JONE_CLK_128 = not1(BYFE_CLK_128);
  /*#p13.KADO*/ wire KADO_RSTn = not1(KEBA_APU_RSTp);
  /*#p13.KALY*/ KALY_CLK_64.dff17(JONE_CLK_128, KADO_RSTn, KALY_CLK_64.qn_old());
  /*#p13.KERE*/ wire KERE_CLK_64 = not1(KALY_CLK_64.qp_new());
  /*#p13.JOLA*/ wire JOLA_CLK_64  = not1(KERE_CLK_64);

  /*#p13.JOVA*/ JOVA_ENV_DELAY0.dff20(JOLA_CLK_64,              KUXU_ENV_DELAY_LOAD, JUSA_NR12_DELAY0.qp_old());
  /*#p13.KENU*/ KENU_ENV_DELAY1.dff20(JOVA_ENV_DELAY0.qp_new(), KUXU_ENV_DELAY_LOAD, JUZY_NR12_DELAY1.qp_old());
  /*#p13.KERA*/ KERA_ENV_DELAY2.dff20(KENU_ENV_DELAY1.qp_new(), KUXU_ENV_DELAY_LOAD, JOMA_NR12_DELAY2.qp_old());

  /*#p13.KOTE*/ wire KOTE_ENV_TRIGGER = and3(KERA_ENV_DELAY2.qp_new(), KENU_ENV_DELAY1.qp_new(), JOVA_ENV_DELAY0.qp_new());

  /*#p13.HUFU*/ wire HUFU = nand5(JAFY_NR12_ENV_DIR.qp_new(), HAFO_ENV3.qp_new(), HEMY_ENV2.qp_new(), HOKO_ENV1.qp_new(), HEVO_ENV0.qp_new());
  /*#p13.HAKE*/ wire HAKE_ENV_TOP = not1(HUFU);
  /*#p13.HANO*/ wire HANO_ENV_BOT = nor5(JAFY_NR12_ENV_DIR.qp_new(), HAFO_ENV3.qp_new(), HEMY_ENV2.qp_new(), HOKO_ENV1.qp_new(), HEVO_ENV0.qp_new());
  /*#p13.JADE*/ wire JADE_ENV_MAX = or2(HANO_ENV_BOT, HAKE_ENV_TOP);

  /*#p13.KOMA*/ wire KOMA_ENV_EN = nor3(JUSA_NR12_DELAY0.qn_new(), JUZY_NR12_DELAY1.qn_new(), JOMA_NR12_DELAY2.qn_new());

  /*#p13.KURY*/ wire KURY = not1(KOZY_ENV_CLK.qp_new());
  /*#p13.KUKU*/ wire KUKU = nor2(HORU_CLK_512, KURY);
  /*#p13.KORO*/ wire KORO_ENV_CLK_RST = nor4(KUKU, KOMA_ENV_EN, FEKU_RESTART0.qp_new(), KEBA_APU_RSTp);

  /*#p13.KOZY*/ KOZY_ENV_CLK.dff17(HORU_CLK_512, KORO_ENV_CLK_RST, KOTE_ENV_TRIGGER);
  /*#p13.KORU*/ wire KORU = nor2(FEKU_RESTART0.qp_new(), KEBA_APU_RSTp);
  /*#p13.KYNO*/ KYNO_ENV_STOP.dff17(KOZY_ENV_CLK.qp_new(), KORU, JADE_ENV_MAX);

  /*#p13.KEKO*/ wire KEKO = or2(KEBA_APU_RSTp, FEKU_RESTART0.qp_new());

  /*#p13.KEZU*/ KEZU.nor_latch(KYNO_ENV_STOP.qp_new(), KEKO);

  /*#p13.KAKE*/ wire KAKE = and3(KOZY_ENV_CLK.qp_new(), KOMA_ENV_EN, KEZU.qp_new());

  /*#p13.HESU*/ wire HESU_ENV_DELTA0 = amux2(JAFY_NR12_ENV_DIR.qn_new(), HOKO_ENV1.qp_new(), HOKO_ENV1.qn_new(), JAFY_NR12_ENV_DIR.qp_new());
  /*#p13.HETO*/ wire HETO_ENV_DELTA1 = amux2(JAFY_NR12_ENV_DIR.qn_new(), HEMY_ENV2.qp_new(), HEMY_ENV2.qn_new(), JAFY_NR12_ENV_DIR.qp_new());
  /*#p13.HYTO*/ wire HYTO_ENV_DELTA2 = amux2(JAFY_NR12_ENV_DIR.qn_new(), HAFO_ENV3.qp_new(), HAFO_ENV3.qn_new(), JAFY_NR12_ENV_DIR.qp_new());
  /*#p13.JUFY*/ wire JUFY_ENV_DELTA3 = amux2(JAFY_NR12_ENV_DIR.qn_new(), KAKE,               KAKE,               JAFY_NR12_ENV_DIR.qp_new());

  // these were connected wrong in schematic
  /*#p13.HEVO*/ HEVO_ENV0.dff20(HESU_ENV_DELTA0, FEKU_RESTART0.qp_new(), JOPU_NR12_VOL3.qn_old());
  /*#p13.HOKO*/ HOKO_ENV1.dff20(HETO_ENV_DELTA1, FEKU_RESTART0.qp_new(), JENA_NR12_VOL2.qn_old());
  /*#p13.HEMY*/ HEMY_ENV2.dff20(HYTO_ENV_DELTA2, FEKU_RESTART0.qp_new(), JAXO_NR12_VOL1.qn_old());
  /*#p13.HAFO*/ HAFO_ENV3.dff20(JUFY_ENV_DELTA3, FEKU_RESTART0.qp_new(), JATY_NR12_VOL0.qn_old());

  //----------
  // Audio output

  /*#p13.BONE*/ wire BONE = not1(ATYS_FREQ_OVERFLOW);
  /*#p13.CYFA*/ wire CYFA = and2(CERO_CH1_LEN_DONE.qp_new(), BOKO_NR14_STOP.qn_new());
  /*#p13.BERY*/ wire BERY_CH1_STOP = or4(BONE, KEBA_APU_RSTp, CYFA, HOCA_CH1_AMP_ENn);

  /*#p13.CYTO*/ CYTO_CH1_ACTIVEp.nor_latch(FEKU_RESTART0.qp_new(), BERY_CH1_STOP);

  /*#p13.COWE*/ wire COWE = and2(CYTO_CH1_ACTIVEp.qp_new(), DUWO_RAW_BIT_SYNC.qp_new());
  /*#p13.BOTO*/ wire BOTO_BIT_OUT = or2(COWE, EDEK_NR52_DBG_APUn);
  /*#p13.AMOP*/ wire AMOP_CH1_OUT0 = and2(HAFO_ENV3.qp_new(), BOTO_BIT_OUT);
  /*#p13.ASON*/ wire ASON_CH1_OUT1 = and2(HEMY_ENV2.qp_new(), BOTO_BIT_OUT);
  /*#p13.AGOF*/ wire AGOF_CH1_OUT2 = and2(HOKO_ENV1.qp_new(), BOTO_BIT_OUT);
  /*#p13.ACEG*/ wire ACEG_CH1_OUT3 = and2(HEVO_ENV0.qp_new(), BOTO_BIT_OUT);













  /*_p10.TACE*/ wire TACE_AMP_ENn = and4(HOCA_CH1_AMP_ENn, FUTE_CH2_AMP_ENn, GUXE_CH3_AMP_ENna.qp_new(), GEVY_CH4_AMP_ENn);
}

//-----------------------------------------------------------------------------
