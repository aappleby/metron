/// plait_noparse

#include "Sch_Channel2.h"

#include "gates.h"

//-----------------------------------------------------------------------------
// This file should contain the schematics as directly translated to C,
// no modifications or simplifications.

void Channel2_tick() {
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

  wire SYKE_ADDR_FFXX = 0;
  wire UNOR_MODE_DBG2 = 0;
  wire ALUR_SYS_RSTn = true;
  wire TEDO_CPU_RDp = false;
  wire TAPU_CPU_WRp = false;
  wire AZOF_xBxDxFxH = 0;
  wire BUDE_xxxxEFGH = 0;
  wire UMER_DIV10n_old = 0;
  wire EDEK_NR52_DBG_APU = 0;
  wire KEBA_APU_RSTp = false;
  wire AMUK_xBxDxFxH = 0;
  wire BYFE_CLK_128 = 0;
  wire DOVA_ABCDxxxx = 0;
  wire BUFY_CLK_256 = 0;
  wire SIG_IN_CPU_DBUS_FREE = 0;
  wire HORU_CLK_512 = 0;

  //----------

  DFF20 JORE;
  DFF20 JONA;
  DFF20 JEVY;
  DFF17 JOPA;

  DFF9 FOFE_NR23_FREQ0;
  DFF9 FOVA_NR23_FREQ1;
  DFF9 FEDY_NR23_FREQ2;
  DFF9 FOME_NR23_FREQ3;
  DFF9 FORA_NR23_FREQ4;
  DFF9 GODA_NR23_FREQ5;
  DFF9 GUMY_NR23_FREQ6;
  DFF9 GUPU_NR23_FREQ7;

  DFF9 BERA_NR21_DUTY0;
  DFF9 BAMY_NR21_DUTY1;

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

  DFF9 JEFU_NR24_FREQ8;
  DFF9 JANY_NR24_FREQ9;
  DFF9 JUPY_NR24_FREQ10;

  DFF20 ERYC_00;
  DFF20 CERA_01;
  DFF20 CONU_02;
  DFF20 CAME_03;
  DFF20 BUVA_04;
  DFF20 AKYD_05;

  DFF17 DOME_WAVE;

  /*_p14.HYFU*/ DFF9 HYFU_NR22_D0;
  /*_p14.HORE*/ DFF9 HORE_NR22_D1;
  /*_p14.HAVA*/ DFF9 HAVA_NR22_D2;
  /*_p14.FORE*/ DFF9 FORE_NR22_D3;
  /*_p14.GATA*/ DFF9 GATA_NR22_D4;
  /*_p14.GUFE*/ DFF9 GUFE_NR22_D5;
  /*_p14.GURA*/ DFF9 GURA_NR22_D6;
  /*_p14.GAGE*/ DFF9 GAGE_NR22_D7;





  /*_p14.JYBU*/ wire JYBU_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p15.KATY*/ wire KATY_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p15.CEXE*/ wire CEXE_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p15.BUWE*/ wire BUWE_APU_RSTn = not1(KEBA_APU_RSTp);
  /*_p15.BYHO*/ wire BYHO_APU_RSTp = not1(BUWE_APU_RSTn);
  /*#p09.AFAT*/ wire AFAT_APU_RSTn = not1(KEBA_APU_RSTp);

  DFF17 CEMO_1MHZ;
  {
    DFF17 ATEP_2MHZ;
    /*_p15.AZEG*/ wire AZEG = not1(AMUK_xBxDxFxH);
    /*_p15.ATEP*/ ATEP_2MHZ.dff17(AZEG, BUWE_APU_RSTn, ATEP_2MHZ.qn_old());
    /*#p15.BUFO*/ wire BUFO = not1(ATEP_2MHZ.qp_new());
    /*#p15.CEMO*/ CEMO_1MHZ.dff17(BUFO, BYHO_APU_RSTp, CEMO_1MHZ.qn_old());
  }

  //----------
  // FF16 NR21

  /*_p10.ATUP*/ wire ATUP_A04n = not1(BUS_CPU_A04p.out_new());
  /*_p07.BAKO*/ wire BAKO_ADDR_FFXXn = not1(SYKE_ADDR_FFXX);
  /*_p10.ATEG*/ wire ATEG_ADDR_XX1Xn = or4(BUS_CPU_A07p.out_new(), BUS_CPU_A06p.out_new(), BUS_CPU_A05p.out_new(), ATUP_A04n);
  /*_p10.DYTE*/ wire DYTE_ADDR_xxx0  = not1(BUS_CPU_A00p.out_new());
  /*_p10.AFOB*/ wire AFOB_ADDR_xx0x  = not1(BUS_CPU_A01p.out_new());
  /*_p10.ABUB*/ wire ABUB_ADDR_x0xx  = not1(BUS_CPU_A02p.out_new());
  /*_p10.ACOL*/ wire ACOL_ADDR_0xxx  = not1(BUS_CPU_A03p.out_new());
  /*_p10.DOSO*/ wire DOSO_ADDR_xxx1  = not1(DYTE_ADDR_xxx0);
  /*_p10.DUPA*/ wire DUPA_ADDR_xx1x  = not1(AFOB_ADDR_xx0x);
  /*_p10.DENO*/ wire DENO_ADDR_x1xx  = not1(ABUB_ADDR_x0xx);
  /*_p10.DUCE*/ wire DUCE_ADDR_1xxx  = not1(ACOL_ADDR_0xxx);
  /*_p10.BUNO*/ wire BUNO_ADDR_FF1Xp = nor2(BAKO_ADDR_FFXXn, ATEG_ADDR_XX1Xn);
  /*_p10.BANU*/ wire BANU_ADDR_FF1Xn = not1(BUNO_ADDR_FF1Xp);
  /*_p10.DAZA*/ wire DAZA_ADDR_0110n = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, DUPA_ADDR_xx1x, DYTE_ADDR_xxx0);
  /*_p10.COVY*/ wire COVY_ADDR_FF16p   = nor2(BANU_ADDR_FF1Xn, DAZA_ADDR_0110n);

  /*_p10.BAFU*/ wire BAFU_CPU_WRn = not1(TAPU_CPU_WRp);
  /*_p10.BOGY*/ wire BOGY_CPU_WRp = not1(BAFU_CPU_WRn);

  /*_p09.AGUZ*/ wire AGUZ_CPU_RDn = not1(TEDO_CPU_RDp);


  /*#p09.AFAT*/ wire AFAT_APU_RESETn = not1(KEBA_APU_RSTp);



  //----------
  // FF17 NR22

  {
    /*_p10.DUVU*/ wire DUVU_ADDR_0111an = nand4(ACOL_ADDR_0xxx, DENO_ADDR_x1xx, DUPA_ADDR_xx1x, DOSO_ADDR_xxx1);
    /*_p10.DUTU*/ wire DUTU_ADDR_FF17  = nor2(BANU_ADDR_FF1Xn, DUVU_ADDR_0111an);

    {
      // FIXME wtf with the different clock polarities? i guess it doesn't matter much.

      /*#p14.ENUF*/ wire ENUF = and2(DUTU_ADDR_FF17, BOGY_CPU_WRp);
      /*#p14.GERE*/ wire GERE = and2(BOGY_CPU_WRp, DUTU_ADDR_FF17);
      /*#p14.JEDE*/ wire JEDE = not1(GERE);

      /*_p14.HYFU*/ HYFU_NR22_D0.dff9(JEDE, JYBU_APU_RSTn, BUS_CPU_D00p.out_old());
      /*_p14.HORE*/ HORE_NR22_D1.dff9(JEDE, JYBU_APU_RSTn, BUS_CPU_D01p.out_old());
      /*_p14.HAVA*/ HAVA_NR22_D2.dff9(JEDE, JYBU_APU_RSTn, BUS_CPU_D02p.out_old());
      /*_p14.FORE*/ FORE_NR22_D3.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D03p.out_old());
      /*_p14.GATA*/ GATA_NR22_D4.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D04p.out_old());
      /*_p14.GUFE*/ GUFE_NR22_D5.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D05p.out_old());
      /*_p14.GURA*/ GURA_NR22_D6.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D06p.out_old());
      /*_p14.GAGE*/ GAGE_NR22_D7.dff9(ENUF, JYBU_APU_RSTn, BUS_CPU_D07p.out_old());
    }

    {
      /*_p14.GURE*/ wire GURE = not1(DUTU_ADDR_FF17);
      /*_p14.FYRY*/ wire FYRY = not1(DUTU_ADDR_FF17);
      /*_p14.GEXA*/ wire GEXA = or2(GURE, AGUZ_CPU_RDn);
      /*_p14.GURU*/ wire GURU = or2(FYRY, AGUZ_CPU_RDn);

      /*_p14.HUVU*/ triwire HUVU_D0 = tri6_nn(GEXA, HYFU_NR22_D0.qp_new());
      /*_p14.HYRE*/ triwire HYRE_D1 = tri6_nn(GEXA, HORE_NR22_D1.qp_new());
      /*_p14.HAVU*/ triwire HAVU_D2 = tri6_nn(GEXA, HYFU_NR22_D0.qp_new());
      /*_p14.GENE*/ triwire GENE_D3 = tri6_nn(GURU, FORE_NR22_D3.qp_new());
      /*_p14.HUPE*/ triwire HUPE_D4 = tri6_nn(GURU, GATA_NR22_D4.qp_new());
      /*_p14.HERE*/ triwire HERE_D5 = tri6_nn(GURU, GUFE_NR22_D5.qp_new());
      /*_p14.HORO*/ triwire HORO_D6 = tri6_nn(GURU, GURA_NR22_D6.qp_new());
      /*_p14.HYRY*/ triwire HYRY_D7 = tri6_nn(GURU, GAGE_NR22_D7.qp_new());

      BUS_CPU_D00p.tri_bus(HUVU_D0);
      BUS_CPU_D01p.tri_bus(HYRE_D1);
      BUS_CPU_D02p.tri_bus(HAVU_D2);
      BUS_CPU_D03p.tri_bus(GENE_D3);
      BUS_CPU_D04p.tri_bus(HUPE_D4);
      BUS_CPU_D05p.tri_bus(HERE_D5);
      BUS_CPU_D06p.tri_bus(HORO_D6);
      BUS_CPU_D07p.tri_bus(HYRY_D7);
    }
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
    /*_p14.FOFE*/ FOFE_NR23_FREQ0.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D00p.out_old());
    /*_p14.FOVA*/ FOVA_NR23_FREQ1.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D01p.out_old());
    /*_p14.FEDY*/ FEDY_NR23_FREQ2.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D02p.out_old());
    /*_p14.FOME*/ FOME_NR23_FREQ3.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D03p.out_old());
    /*_p14.FORA*/ FORA_NR23_FREQ4.dff9(ESUR_NR23_WRn1, HUDE_APU_RESETn1, BUS_CPU_D04p.out_old());
    /*_p14.GODA*/ GODA_NR23_FREQ5.dff9(FYXO_NR23_WRn2, HUDE_APU_RESETn1, BUS_CPU_D05p.out_old());
    /*_p14.GUMY*/ GUMY_NR23_FREQ6.dff9(FYXO_NR23_WRn2, HUDE_APU_RESETn1, BUS_CPU_D06p.out_old());
    /*_p14.GUPU*/ GUPU_NR23_FREQ7.dff9(FYXO_NR23_WRn2, HUDE_APU_RESETn1, BUS_CPU_D07p.out_old());
  }

  /*_p14.FOGE*/ wire FOGE = not1(AGUZ_CPU_RDn);
  /*_p14.FAPE*/ wire FAPE = and2(FOGE, EDEK_NR52_DBG_APU);

  {
    /*_p14.FERY*/ wire FERY = not1(DUCE_ADDR_1xxx);
    /*_p14.GUZA*/ wire GUZA = nor2(FERY, FAPE);
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

  DFF17 ELOX;
  DFF17 CAZA;
  DFF9 EMER_NR24_STOP;
  {
    DFF17 DOPE_START_SYNC;

    /*_p10.DEJY*/ wire DEJY_ADDR_1001an = nand4(DUCE_ADDR_1xxx, ABUB_ADDR_x0xx, AFOB_ADDR_xx0x, DOSO_ADDR_xxx1);
    /*_p10.DOZA*/ wire DOZA_ADDR_FF19  = nor2(BANU_ADDR_FF1Xn, DEJY_ADDR_1001an);

    {
      /*_pXX.JENU*/ wire JENU = and2(DOZA_ADDR_FF19, BOGY_CPU_WRp);
      /*_pXX.KYSA*/ wire KYSA = not1(JENU);
      /*_p14.KYPU*/ wire KYPU_APU_RSTn = not1(KEBA_APU_RSTp);
      /*_pXX.JEFU*/ JEFU_NR24_FREQ8  .dff9(KYSA, KYPU_APU_RSTn, BUS_CPU_D00p.out_old());
      /*_pXX.JANY*/ JANY_NR24_FREQ9  .dff9(KYSA, KYPU_APU_RSTn, BUS_CPU_D01p.out_old());
      /*_pXX.JUPY*/ JUPY_NR24_FREQ10 .dff9(KYSA, KYPU_APU_RSTn, BUS_CPU_D02p.out_old());
    }

    {
      /*_p16.ANUJ*/ wire ANUJ_CPU_WR_WEIRD = and2(SIG_IN_CPU_DBUS_FREE, BOGY_CPU_WRp);
      /*_pXX.EVYF*/ wire EVYF = nand2(ANUJ_CPU_WR_WEIRD, DOZA_ADDR_FF19);
      /*_p14.FAZO*/ wire FAZO_APU_RSTn = not1(KEBA_APU_RSTp);
      /*_pXX.EMER*/ EMER_NR24_STOP.dff9(EVYF, FAZO_APU_RSTn, BUS_CPU_D06p.out_old());

      /*#pXX.GADO*/ wire GADO = not1(AGUZ_CPU_RDn);
      /*#pXX.HUMA*/ wire HUMA = nand2(DOZA_ADDR_FF19, GADO); // why was this nor2? doublecheck
      /*#pXX.GOJY*/ triwire GOJY_D6 = tri6_nn(HUMA, EMER_NR24_STOP.qp_new());
      BUS_CPU_D06p.tri_bus(GOJY_D6);
    }

    {
      DFF9 ETAP_NR24_START;
      /*#p??.DETA*/ wire DETA = nand2(BOGY_CPU_WRp, DOZA_ADDR_FF19);
      /*#p15.DERA*/ wire DERA = nor2(KEBA_APU_RSTp, DOPE_START_SYNC.qp_new());
      /*#p??.ETAP*/ ETAP_NR24_START.dff9(DETA, DERA, BUS_CPU_D07p.out_old());

      /*_p15.CYWU*/ wire CYWU_APU_RSTn = not1(KEBA_APU_RSTp);
      /*#p15.DOPE*/ DOPE_START_SYNC.dff17(DOVA_ABCDxxxx, CYWU_APU_RSTn, ETAP_NR24_START.qn_old());
    }

    {
      /*#p14.GOTE*/ wire GOTE = not1(DOZA_ADDR_FF19);
      /*#p14.HYPO*/ wire HYPO = or2(GOTE, FAPE);
      /*#p14.HUNA*/ triwire HUNA_D0 = tri6_nn(HYPO, HEVY_CH2_FREQ_08.qn_new());
      /*#p14.JARO*/ triwire JARO_D1 = tri6_nn(HYPO, HEPU_CH2_FREQ_09.qn_new());
      /*#p14.JEKE*/ triwire JEKE_D2 = tri6_nn(HYPO, HERO_CH2_FREQ_10.qn_new());

      BUS_CPU_D00p.tri_bus(HUNA_D0);
      BUS_CPU_D01p.tri_bus(JARO_D1);
      BUS_CPU_D02p.tri_bus(JEKE_D2);
    }

    DFF17 DORY;
    /*_p15.DOXA*/ wire DOXA = or2(KEBA_APU_RSTp, DORY.qp_new());
    /*_p15.CELO*/ wire CELO = not1(DOXA);
    /*_p15.DALA*/ wire DALA = or2(CELO, DOPE_START_SYNC.qp_new());
    /*_p15.ELOX*/ ELOX.dff17(CEMO_1MHZ.qp_new(), DOXA,          DALA);
    /*_p15.DORY*/ DORY.dff17(CEMO_1MHZ.qp_new(), CEXE_APU_RSTn, ELOX.qp_new());
    /*_p15.CAZA*/ CAZA.dff17(CEMO_1MHZ.qp_new(), CEXE_APU_RSTn, DORY.qp_old());
  }



  // weird latch?


  /*#p15.FUTE*/ wire FUTE_CH2_AMP_ENn = nor5(FORE_NR22_D3.qn_new(), GATA_NR22_D4.qn_new(), GUFE_NR22_D5.qn_new(), GURA_NR22_D6.qn_new(), GAGE_NR22_D7.qn_new());

  // this is freq overflow or something
  DFF17 GYKO;
  {
    /*#p15.FUJY*/ wire FUJY = and2(GYKO.qp_new(), CEMO_1MHZ.qp_new());
    /*#p15.GALU*/ wire GALU = not1(HERO_CH2_FREQ_10.qp_new());
    /*#p15.GYRE*/ wire GYRE = nor3(KEBA_APU_RSTp, ELOX.qp_new(), FUJY);
    /*#p15.GYKO*/ GYKO.dff17(GALU, GYRE, GYKO.qn_old());
  }
  /*#p15.ETUK*/ wire ETUK = not1(GYKO.qp_new());
  /*#p15.DAVU*/ wire DAVU = not1(ETUK);


  DFF17 CANO_00;
  DFF13 CAGY_01;
  DFF13 DYVE_02;

  {
    /*#p15.CULE*/ wire CULE = not1(DAVU);
    /*#p15.CANO*/ CANO_00.dff17(CULE,             AFAT_APU_RESETn, CANO_00.qn_old());
    /*#p15.CAGY*/ CAGY_01.dff13(CANO_00.qn_new(), AFAT_APU_RESETn, CAGY_01.qn_old());
    /*#p15.DYVE*/ DYVE_02.dff13(CAGY_01.qn_new(), AFAT_APU_RESETn, DYVE_02.qn_old());
  }

  {

    NandLatch BUTA_FREQ_GATEp; // check if this is nor or nand

    /*#p15.DUJU*/ wire DUJU_LOADn = nor2(DAVU, ELOX.qp_new());
    /*#p15.COGU*/ wire COGU_LOADp = not1(DUJU_LOADn);
    /*#p15.EROG*/ wire EROG_LOADp = not1(DUJU_LOADn);
    /*#p15.GYPA*/ wire GYPA_LOADp = not1(DUJU_LOADn);

    /*#p15.ARES*/ wire ARES = nor2(FUTE_CH2_AMP_ENn, KEBA_APU_RSTp); // looks like nor on the die?
    /*#p15.BODO*/ wire BODO = not1(CAZA.qp_new());
    /*_p15.BUTA*/ BUTA_FREQ_GATEp.nand_latch(ARES, BODO);
    /*#p15.CAMA*/ wire CAMA_FREQ_TICKp = nor2(CEMO_1MHZ.qp_new(), BUTA_FREQ_GATEp.qp_new()); // no idea of buta polarity
    /*_p15.DOCA*/ wire DOCA_FREQ_TICKn = not1(CAMA_FREQ_TICKp);

    /*_p14.DONE*/ DONE_CH2_FREQ_00.dff20(DOCA_FREQ_TICKn,            COGU_LOADp, FOFE_NR23_FREQ0.qp_new());
    /*_p14.DYNU*/ DYNU_CH2_FREQ_01.dff20(DONE_CH2_FREQ_00.qp_new(),  COGU_LOADp, FOVA_NR23_FREQ1.qp_new());
    /*_p14.EZOF*/ EZOF_CH2_FREQ_02.dff20(DYNU_CH2_FREQ_01.qp_new(),  COGU_LOADp, FEDY_NR23_FREQ2.qp_new());
    /*_p14.CYVO*/ CYVO_CH2_FREQ_03.dff20(EZOF_CH2_FREQ_02.qp_new(),  COGU_LOADp, FOME_NR23_FREQ3.qp_new());

    /*#p14.EDEP*/ wire EDEP_CH2_FREQ_03 = not1(CYVO_CH2_FREQ_03.qn_new());
    /*_p14.FUXO*/ FUXO_CH2_FREQ_04.dff20(EDEP_CH2_FREQ_03,           EROG_LOADp, FORA_NR23_FREQ4.qp_new());
    /*_p14.GANO*/ GANO_CH2_FREQ_05.dff20(FUXO_CH2_FREQ_04.qp_new(),  EROG_LOADp, GODA_NR23_FREQ5.qp_new());
    /*_p14.GOCA*/ GOCA_CH2_FREQ_06.dff20(GANO_CH2_FREQ_05.qp_new(),  EROG_LOADp, GUMY_NR23_FREQ6.qp_new());
    /*_p14.GANE*/ GANE_CH2_FREQ_07.dff20(GOCA_CH2_FREQ_06.qp_new(),  EROG_LOADp, GUPU_NR23_FREQ7.qp_new());

    /*#p14.GALA*/ wire GALA_CH2_FREQ_07 = not1(GANE_CH2_FREQ_07.qn_new());
    /*_p14.HEVY*/ HEVY_CH2_FREQ_08.dff20(GALA_CH2_FREQ_07,           GYPA_LOADp, JEFU_NR24_FREQ8.qp_new());
    /*_p14.HEPU*/ HEPU_CH2_FREQ_09.dff20(HEVY_CH2_FREQ_08.qp_new(),  GYPA_LOADp, JANY_NR24_FREQ9.qp_new());
    /*_p14.HERO*/ HERO_CH2_FREQ_10.dff20(HEPU_CH2_FREQ_09.qp_new(),  GYPA_LOADp, JUPY_NR24_FREQ10.qp_new());

  }


  //----------

  DFF20 FENA;
  DFF20 FOMY;
  DFF20 FETE;
  DFF20 FENO;
  DFF17 HEPO;
  NorLatch JEME;

  {
    DFF17 JYNA;
    /*#p15.HOTA*/ wire HOTA = not1(BYFE_CLK_128);
    /*#p15.JYNA*/ JYNA.dff17(HOTA, KATY_APU_RSTn, JYNA.qn_old());
    /*#p15.KYLO*/ wire KYLO = not1(JYNA.qp_new());
    /*#p15.KENE*/ wire KENE = not1(KYLO);
    /*_p15.HYLY*/ wire HYLY = nor2(ELOX.qp_new(), JOPA.qp_new());
    /*_p15.JAKE*/ wire JAKE = not1(HYLY);
    /*#p15.JORE*/ JORE.dff20(KENE,          JAKE, HYFU_NR22_D0.qp_new());
    /*#p15.JONA*/ JONA.dff20(JORE.qp_new(), JAKE, HORE_NR22_D1.qp_new());
    /*#p15.JEVY*/ JEVY.dff20(JONA.qp_new(), JAKE, HAVA_NR22_D2.qp_new());
  }

  /*#p15.JUPU*/ wire JUPU = nor3(HYFU_NR22_D0.qn_new(), HORE_NR22_D1.qn_new(), HAVA_NR22_D2.qn_new());

  {
    /*#p15.GADE*/ wire GADE = not1(JOPA.qp_new());
    /*#p15.HOLY*/ wire HOLY = nor2(HORU_CLK_512, GADE);
    /*#p15.HAFE*/ wire HAFE = nor4(HOLY, JUPU, ELOX.qp_new(), KEBA_APU_RSTp); // schematic wrong this is nor4
    /*#p15.KYVO*/ wire KYVO = and3(JEVY.qp_new(), JONA.qp_new(), JORE.qp_new());
    /*#p15.JOPA*/ JOPA.dff17(HORU_CLK_512, HAFE, KYVO);
  }


  {
    /*_p15.HYPA*/ wire HYPA = nor2(ELOX.qp_new(), KEBA_APU_RSTp);
    /*#p15.EMYR*/ wire EMYR = nor5 (FORE_NR22_D3.qn_new(), FENO.qp_new(), FETE.qp_new(), FOMY.qp_new(), FENA.qp_new());
    /*#p15.ERAT*/ wire ERAT = nand5(FORE_NR22_D3.qn_new(), FENO.qp_new(), FETE.qp_new(), FOMY.qp_new(), FENA.qp_new());
    /*#p15.FYRE*/ wire FYRE = not1(ERAT);
    /*#p15.GUFY*/ wire GUFY = or2(EMYR, FYRE);
    /*#p15.HEPO*/ HEPO.dff17(JOPA.qp_new(), HYPA, GUFY);
    /*_p15.HYLE*/ wire HYLE = or2(KEBA_APU_RSTp, ELOX.qp_new());
    /*#p15.JEME*/ JEME.nor_latch(HEPO.qp_new(), HYLE);
  }
  
  
  

  NorLatch DANE_CH2_ACTIVE_LATCHp;

  // something to do with ff16
  {
    DFF17 CYRE;
    /*_p15.DORA*/ wire DORA = and2(CYRE.qp_new(), EMER_NR24_STOP.qn_new());
    /*_p15.ESYK*/ wire ESYK = or3(KEBA_APU_RSTp, DORA, FUTE_CH2_AMP_ENn);
    /*_p15.DANE*/ DANE_CH2_ACTIVE_LATCHp.nor_latch(ELOX.qp_new(), ESYK);

    /*_p14.AGYN*/ wire AGYN_FF16_WRn = nand2(BOGY_CPU_WRp, COVY_ADDR_FF16p);
    /*_p15.DEME*/ wire DEME = nor3(CYRE.qp_new(), BUFY_CLK_256, EMER_NR24_STOP.qp_new()); // why was this or2?
    /*_p15.DYRO*/ wire DYRO = not1(DEME);
    /*_p15.BYMO*/ wire BYMO = not1(AGYN_FF16_WRn);
    /*_p15.ERYC*/ ERYC_00.dff20(DYRO,              BYMO, BUS_CPU_D00p.out_old());
    /*_p15.CERA*/ CERA_01.dff20(ERYC_00.qp_new(),  BYMO, BUS_CPU_D01p.out_old());
    /*_p15.CONU*/ CONU_02.dff20(CERA_01.qp_new(),  BYMO, BUS_CPU_D02p.out_old());
    /*_p15.CAME*/ CAME_03.dff20(CONU_02.qp_new(),  BYMO, BUS_CPU_D03p.out_old());

    /*#p15.BUKO*/ wire BUKO = not1(CAME_03.qn_new());
    /*_p15.AGET*/ wire AGET = not1(AGYN_FF16_WRn);
    /*_p15.BUVA*/ BUVA_04.dff20(BUKO,              AGET, BUS_CPU_D04p.out_old());
    /*_p15.AKYD*/ AKYD_05.dff20(BUVA_04.qp_new(),  AGET, BUS_CPU_D05p.out_old());

    // what is this thing?
    /*_p14.ASYP*/ wire ASYP = not1(AGYN_FF16_WRn);
    /*_p14.BENY*/ wire BENY = nor3(ASYP, KEBA_APU_RSTp, ELOX.qp_new());
    /*_p15.CYRE*/ CYRE.dff17(AKYD_05.qn_new(), BENY, CYRE.qn_old());
  }









  {
    /*_p14.BACU*/ wire BACU = and2(COVY_ADDR_FF16p, BOGY_CPU_WRp);
    /*_p14.BUDU*/ wire BUDU = not1(BACU);
    /*_p14.BERA*/ BERA_NR21_DUTY0.dff9(BUDU, AFAT_APU_RESETn, BUS_CPU_D00p.out_old());
    /*_p14.BAMY*/ BAMY_NR21_DUTY1.dff9(BUDU, AFAT_APU_RESETn, BUS_CPU_D01p.out_old());

    /*_p14.BYGO*/ wire BYGO = not1(AGUZ_CPU_RDn);
    /*_p14.CORO*/ wire CORO = nand2(COVY_ADDR_FF16p, BYGO);
    /*_p14.CECY*/ triwire CECY_D6 = tri6_nn(CORO, BERA_NR21_DUTY0.qp_new());
    /*_p14.CEKA*/ triwire CEKA_D7 = tri6_nn(CORO, BAMY_NR21_DUTY1.qp_new());

    BUS_CPU_D06p.tri_bus(CECY_D6);
    BUS_CPU_D07p.tri_bus(CEKA_D7);
  }

  {
    /*_p15.HOFO*/ wire HOFO = or3(JOPA.qp_new(), JUPU, JEME.qn_new()); // no idea of jeme polarity
    /*#p15.GAFA*/ wire GAFA = amux2(FORE_NR22_D3.qn_new(), HOFO,          HOFO,          FORE_NR22_D3.qp_new());
    /*#p15.FARU*/ wire FARU = amux2(FORE_NR22_D3.qn_new(), FENO.qp_new(), FENO.qn_new(), FORE_NR22_D3.qp_new());
    /*#p15.ETUP*/ wire ETUP = amux2(FORE_NR22_D3.qn_new(), FETE.qp_new(), FETE.qn_new(), FORE_NR22_D3.qp_new());
    /*#p15.FOPY*/ wire FOPY = amux2(FORE_NR22_D3.qn_new(), FOMY.qp_new(), FOMY.qn_new(), FORE_NR22_D3.qp_new());

    /*#p15.FENO*/ FENO.dff20(GAFA, ELOX.qp_new(), FENO.qn_old());
    /*#p15.FETE*/ FETE.dff20(FARU, ELOX.qp_new(), GUFE_NR22_D5.qn_old());
    /*#p15.FOMY*/ FOMY.dff20(ETUP, ELOX.qp_new(), GURA_NR22_D6.qn_old());
    /*#p15.FENA*/ FENA.dff20(FOPY, ELOX.qp_new(), GAGE_NR22_D7.qn_old());

    /*#p15.DYTA*/ wire DYTA = nor2(BERA_NR21_DUTY0.qp_new(), BAMY_NR21_DUTY1.qn_new());
    /*#p15.DOMO*/ wire DOMO = nor2(BERA_NR21_DUTY0.qn_new(), BAMY_NR21_DUTY1.qn_new());
    /*#p15.DOJU*/ wire DOJU = nor2(BERA_NR21_DUTY0.qn_new(), BAMY_NR21_DUTY1.qp_new());
    /*#p15.DOVE*/ wire DOVE = nor2(BERA_NR21_DUTY0.qp_new(), BAMY_NR21_DUTY1.qp_new());

    /*#p15.DYMU*/ wire DYMU = and2(DYVE_02.qp_new(), CAGY_01.qp_new());
    /*#p15.DARE*/ wire DARE = not1(DYMU);
    /*#p15.DUGE*/ wire DUGE = not1(CANO_00.qp_new());
    /*#p15.EGOG*/ wire EGOG = and2(DYMU, DUGE);
    
    /*#p15.EXES*/ wire EXES_WAVE = amux4(EGOG, DOMO,
                                         DYMU, DYTA,
                                         DYVE_02.qp_new(), DOJU,
                                         DARE, DOVE);

    /*#p15.DOME*/ DOME_WAVE.dff17(DAVU, AFAT_APU_RSTn, EXES_WAVE);
    /*#p15.CYSE*/ wire CYSE = and2(DANE_CH2_ACTIVE_LATCHp.qp_new(), DOME_WAVE.qp_new()); // no idea of dane polarity
    /*#p15.BONU*/ wire BONU = or2(CYSE, EDEK_NR52_DBG_APU);

    // dac 2 inputs ANAN ANYV ASOG AMOV
    /*#p15.ANYV*/ wire ANYV = and2(FETE.qp_new(), BONU);
    /*#p15.ANAN*/ wire ANAN = and2(FENO.qp_new(), BONU);
    /*#p15.AMOV*/ wire AMOV = and2(FENA.qp_new(), BONU);
    /*#p15.ASOG*/ wire ASOG = and2(FOMY.qp_new(), BONU);
  }

  // used by apucontrol
  /*_p15.DEFU*/ wire DEFU_CH2_ACTIVEn = not1(DANE_CH2_ACTIVE_LATCHp.qp_new()); // no idea about dane polarity
}

//-----------------------------------------------------------------------------
