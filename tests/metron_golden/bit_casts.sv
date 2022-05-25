`include "metron_tools.sv"

// Logics can be casted to various sizes via bN() or bx<N>()

module Module (
  // test_bN() ports
  output int test_bN_ret,
  // test_bx_const() ports
  output int test_bx_const_ret,
  // test_bN_offset() ports
  output int test_bN_offset_ret,
  // test_bx_param() ports
  output int test_bx_param_ret,
  // test2() ports
  output int test2_ret
);
/*public:*/

  always_comb begin : test_bN
    logic[63:0] src;
    logic[63:0] dst64;
    logic[62:0] dst63;
    logic[61:0] dst62;
    logic[60:0] dst61;
    logic[59:0] dst60;
    logic[58:0] dst59;
    logic[57:0] dst58;
    logic[56:0] dst57;
    logic[55:0] dst56;
    logic[54:0] dst55;
    logic[53:0] dst54;
    logic[52:0] dst53;
    logic[51:0] dst52;
    logic[50:0] dst51;
    logic[49:0] dst50;
    logic[48:0] dst49;
    logic[47:0] dst48;
    logic[46:0] dst47;
    logic[45:0] dst46;
    logic[44:0] dst45;
    logic[43:0] dst44;
    logic[42:0] dst43;
    logic[41:0] dst42;
    logic[40:0] dst41;
    logic[39:0] dst40;
    logic[38:0] dst39;
    logic[37:0] dst38;
    logic[36:0] dst37;
    logic[35:0] dst36;
    logic[34:0] dst35;
    logic[33:0] dst34;
    logic[32:0] dst33;
    logic[31:0] dst32;
    logic[30:0] dst31;
    logic[29:0] dst30;
    logic[28:0] dst29;
    logic[27:0] dst28;
    logic[26:0] dst27;
    logic[25:0] dst26;
    logic[24:0] dst25;
    logic[23:0] dst24;
    logic[22:0] dst23;
    logic[21:0] dst22;
    logic[20:0] dst21;
    logic[19:0] dst20;
    logic[18:0] dst19;
    logic[17:0] dst18;
    logic[16:0] dst17;
    logic[15:0] dst16;
    logic[14:0] dst15;
    logic[13:0] dst14;
    logic[12:0] dst13;
    logic[11:0] dst12;
    logic[10:0] dst11;
    logic[9:0] dst10;
    logic[8:0] dst9;
    logic[7:0] dst8;
    logic[6:0] dst7;
    logic[5:0] dst6;
    logic[4:0] dst5;
    logic[3:0] dst4;
    logic[2:0] dst3;
    logic[1:0] dst2;
    logic dst1;
    src = 64'h1234567812345678;
    dst64 = 64'(src);
    dst63 = 63'(src);
    dst62 = 62'(src);
    dst61 = 61'(src);
    dst60 = 60'(src);

    dst59 = 59'(src);
    dst58 = 58'(src);
    dst57 = 57'(src);
    dst56 = 56'(src);
    dst55 = 55'(src);
    dst54 = 54'(src);
    dst53 = 53'(src);
    dst52 = 52'(src);
    dst51 = 51'(src);
    dst50 = 50'(src);

    dst49 = 49'(src);
    dst48 = 48'(src);
    dst47 = 47'(src);
    dst46 = 46'(src);
    dst45 = 45'(src);
    dst44 = 44'(src);
    dst43 = 43'(src);
    dst42 = 42'(src);
    dst41 = 41'(src);
    dst40 = 40'(src);

    dst39 = 39'(src);
    dst38 = 38'(src);
    dst37 = 37'(src);
    dst36 = 36'(src);
    dst35 = 35'(src);
    dst34 = 34'(src);
    dst33 = 33'(src);
    dst32 = 32'(src);
    dst31 = 31'(src);
    dst30 = 30'(src);

    dst29 = 29'(src);
    dst28 = 28'(src);
    dst27 = 27'(src);
    dst26 = 26'(src);
    dst25 = 25'(src);
    dst24 = 24'(src);
    dst23 = 23'(src);
    dst22 = 22'(src);
    dst21 = 21'(src);
    dst20 = 20'(src);

    dst19 = 19'(src);
    dst18 = 18'(src);
    dst17 = 17'(src);
    dst16 = 16'(src);
    dst15 = 15'(src);
    dst14 = 14'(src);
    dst13 = 13'(src);
    dst12 = 12'(src);
    dst11 = 11'(src);
    dst10 = 10'(src);

    dst9 = 9'(src);
    dst8 = 8'(src);
    dst7 = 7'(src);
    dst6 = 6'(src);
    dst5 = 5'(src);
    dst4 = 4'(src);
    dst3 = 3'(src);
    dst2 = 2'(src);
    dst1 = 1'(src);
    test_bN_ret = 0;
  end

  always_comb begin : test_bx_const
    logic[63:0] src;
    logic[62:0] dst63;
    logic[61:0] dst62;
    logic[60:0] dst61;
    logic[59:0] dst60;
    logic[58:0] dst59;
    logic[57:0] dst58;
    logic[56:0] dst57;
    logic[55:0] dst56;
    logic[54:0] dst55;
    logic[53:0] dst54;
    logic[52:0] dst53;
    logic[51:0] dst52;
    logic[50:0] dst51;
    logic[49:0] dst50;
    logic[48:0] dst49;
    logic[47:0] dst48;
    logic[46:0] dst47;
    logic[45:0] dst46;
    logic[44:0] dst45;
    logic[43:0] dst44;
    logic[42:0] dst43;
    logic[41:0] dst42;
    logic[40:0] dst41;
    logic[39:0] dst40;
    logic[38:0] dst39;
    logic[37:0] dst38;
    logic[36:0] dst37;
    logic[35:0] dst36;
    logic[34:0] dst35;
    logic[33:0] dst34;
    logic[32:0] dst33;
    logic[31:0] dst32;
    logic[30:0] dst31;
    logic[29:0] dst30;
    logic[28:0] dst29;
    logic[27:0] dst28;
    logic[26:0] dst27;
    logic[25:0] dst26;
    logic[24:0] dst25;
    logic[23:0] dst24;
    logic[22:0] dst23;
    logic[21:0] dst22;
    logic[20:0] dst21;
    logic[19:0] dst20;
    logic[18:0] dst19;
    logic[17:0] dst18;
    logic[16:0] dst17;
    logic[15:0] dst16;
    logic[14:0] dst15;
    logic[13:0] dst14;
    logic[12:0] dst13;
    logic[11:0] dst12;
    logic[10:0] dst11;
    logic[9:0] dst10;
    logic[8:0] dst9;
    logic[7:0] dst8;
    logic[6:0] dst7;
    logic[5:0] dst6;
    logic[4:0] dst5;
    logic[3:0] dst4;
    logic[2:0] dst3;
    logic[1:0] dst2;
    logic dst1;
    src = 64'h1234567812345678;
    dst63 = (63)'(src);
    dst62 = (62)'(src);
    dst61 = (61)'(src);
    dst60 = (60)'(src);

    dst59 = (59)'(src);
    dst58 = (58)'(src);
    dst57 = (57)'(src);
    dst56 = (56)'(src);
    dst55 = (55)'(src);
    dst54 = (54)'(src);
    dst53 = (53)'(src);
    dst52 = (52)'(src);
    dst51 = (51)'(src);
    dst50 = (50)'(src);

    dst49 = (49)'(src);
    dst48 = (48)'(src);
    dst47 = (47)'(src);
    dst46 = (46)'(src);
    dst45 = (45)'(src);
    dst44 = (44)'(src);
    dst43 = (43)'(src);
    dst42 = (42)'(src);
    dst41 = (41)'(src);
    dst40 = (40)'(src);

    dst39 = (39)'(src);
    dst38 = (38)'(src);
    dst37 = (37)'(src);
    dst36 = (36)'(src);
    dst35 = (35)'(src);
    dst34 = (34)'(src);
    dst33 = (33)'(src);
    dst32 = (32)'(src);
    dst31 = (31)'(src);
    dst30 = (30)'(src);

    dst29 = (29)'(src);
    dst28 = (28)'(src);
    dst27 = (27)'(src);
    dst26 = (26)'(src);
    dst25 = (25)'(src);
    dst24 = (24)'(src);
    dst23 = (23)'(src);
    dst22 = (22)'(src);
    dst21 = (21)'(src);
    dst20 = (20)'(src);

    dst19 = (19)'(src);
    dst18 = (18)'(src);
    dst17 = (17)'(src);
    dst16 = (16)'(src);
    dst15 = (15)'(src);
    dst14 = (14)'(src);
    dst13 = (13)'(src);
    dst12 = (12)'(src);
    dst11 = (11)'(src);
    dst10 = (10)'(src);

    dst9 = (9)'(src);
    dst8 = (8)'(src);
    dst7 = (7)'(src);
    dst6 = (6)'(src);
    dst5 = (5)'(src);
    dst4 = (4)'(src);
    dst3 = (3)'(src);
    dst2 = (2)'(src);
    dst1 = (1)'(src);
    test_bx_const_ret = 0;
  end

  always_comb begin : test_bN_offset
    logic[63:0] src;
    logic[7:0] dst0;
    logic[7:0] dst1;
    logic[7:0] dst2;
    logic[7:0] dst3;
    logic[7:0] dst4;
    logic[7:0] dst5;
    logic[7:0] dst6;
    logic[7:0] dst7;
    logic[7:0] dst8;
    logic[7:0] dst9;
    src = 64'h1234567812345678;

    dst0 = src[7:0];
    dst1 = src[8:1];
    dst2 = src[9:2];
    dst3 = src[10:3];
    dst4 = src[11:4];
    dst5 = src[12:5];
    dst6 = src[13:6];
    dst7 = src[14:7];
    dst8 = src[15:8];
    dst9 = src[16:9];
    test_bN_offset_ret = 0;
  end

  localparam int some_size1 = 64;
  localparam int some_size2 = 8;

  always_comb begin : test_bx_param
    logic[some_size1-1:0] a;
    logic[some_size2-1:0] b;
    logic[some_size2-1:0] b0;
    logic[some_size2-1:0] b1;
    logic[some_size2-1:0] b2;
    logic[some_size2-1:0] b3;
    logic[some_size2-1:0] b4;
    logic[some_size2-1:0] b5;
    logic[some_size2-1:0] b6;
    logic[some_size2-1:0] b7;
    logic[some_size2-1:0] b8;
    logic[some_size2-1:0] b9;
    a = 10;
    b = (some_size2)'(a);

    b0 = a[some_size2+-1:0];
    b1 = a[some_size2+0:1];
    b2 = a[some_size2+1:2];
    b3 = a[some_size2+2:3];
    b4 = a[some_size2+3:4];
    b5 = a[some_size2+4:5];
    b6 = a[some_size2+5:6];
    b7 = a[some_size2+6:7];
    b8 = a[some_size2+7:8];
    b9 = a[some_size2+8:9];

    test_bx_param_ret = 0;
  end


  always_comb begin : test2
    logic[31:0] a;
    logic b;
    logic[6:0] c;
    logic e;
    logic[6:0] f;
    a = 32'hDEADBEEF;

    b = a[3]; //static bit extract with literal offset, width 1
    c = a[9:3]; //static bit extract with literal offset, width N

    e = a[some_size1]; //static bit extract with variable offset, width 1
    f = a[6 + some_size2 : some_size2]; //static bit extract with variable offset, width N

    test2_ret = 0;
  end



endmodule
