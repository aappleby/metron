`include "metron_tools.sv"

// Logics can be casted to various sizes via bN() or bx<N>()

module Module
(
  input logic clock
);
/*public:*/

  function void tock_bN();
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

    logic[63:0] src = 64'h1234567812345678;
    logic[63:0] dst64 = 64'(src);
    logic[62:0] dst63 = 63'(src);
    logic[61:0] dst62 = 62'(src);
    logic[60:0] dst61 = 61'(src);
    logic[59:0] dst60 = 60'(src);

    logic[58:0] dst59 = 59'(src);
    logic[57:0] dst58 = 58'(src);
    logic[56:0] dst57 = 57'(src);
    logic[55:0] dst56 = 56'(src);
    logic[54:0] dst55 = 55'(src);
    logic[53:0] dst54 = 54'(src);
    logic[52:0] dst53 = 53'(src);
    logic[51:0] dst52 = 52'(src);
    logic[50:0] dst51 = 51'(src);
    logic[49:0] dst50 = 50'(src);

    logic[48:0] dst49 = 49'(src);
    logic[47:0] dst48 = 48'(src);
    logic[46:0] dst47 = 47'(src);
    logic[45:0] dst46 = 46'(src);
    logic[44:0] dst45 = 45'(src);
    logic[43:0] dst44 = 44'(src);
    logic[42:0] dst43 = 43'(src);
    logic[41:0] dst42 = 42'(src);
    logic[40:0] dst41 = 41'(src);
    logic[39:0] dst40 = 40'(src);

    logic[38:0] dst39 = 39'(src);
    logic[37:0] dst38 = 38'(src);
    logic[36:0] dst37 = 37'(src);
    logic[35:0] dst36 = 36'(src);
    logic[34:0] dst35 = 35'(src);
    logic[33:0] dst34 = 34'(src);
    logic[32:0] dst33 = 33'(src);
    logic[31:0] dst32 = 32'(src);
    logic[30:0] dst31 = 31'(src);
    logic[29:0] dst30 = 30'(src);

    logic[28:0] dst29 = 29'(src);
    logic[27:0] dst28 = 28'(src);
    logic[26:0] dst27 = 27'(src);
    logic[25:0] dst26 = 26'(src);
    logic[24:0] dst25 = 25'(src);
    logic[23:0] dst24 = 24'(src);
    logic[22:0] dst23 = 23'(src);
    logic[21:0] dst22 = 22'(src);
    logic[20:0] dst21 = 21'(src);
    logic[19:0] dst20 = 20'(src);

    logic[18:0] dst19 = 19'(src);
    logic[17:0] dst18 = 18'(src);
    logic[16:0] dst17 = 17'(src);
    logic[15:0] dst16 = 16'(src);
    logic[14:0] dst15 = 15'(src);
    logic[13:0] dst14 = 14'(src);
    logic[12:0] dst13 = 13'(src);
    logic[11:0] dst12 = 12'(src);
    logic[10:0] dst11 = 11'(src);
    logic[9:0] dst10 = 10'(src);

    logic[8:0] dst9 = 9'(src);
    logic[7:0] dst8 = 8'(src);
    logic[6:0] dst7 = 7'(src);
    logic[5:0] dst6 = 6'(src);
    logic[4:0] dst5 = 5'(src);
    logic[3:0] dst4 = 4'(src);
    logic[2:0] dst3 = 3'(src);
    logic[1:0] dst2 = 2'(src);
    logic dst1 = 1'(src);
  endfuction

  function void tock_bx_const();
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

    logic[63:0] src = 64'h1234567812345678;
    logic[62:0] dst63 = (63)'(src);
    logic[61:0] dst62 = (62)'(src);
    logic[60:0] dst61 = (61)'(src);
    logic[59:0] dst60 = (60)'(src);

    logic[58:0] dst59 = (59)'(src);
    logic[57:0] dst58 = (58)'(src);
    logic[56:0] dst57 = (57)'(src);
    logic[55:0] dst56 = (56)'(src);
    logic[54:0] dst55 = (55)'(src);
    logic[53:0] dst54 = (54)'(src);
    logic[52:0] dst53 = (53)'(src);
    logic[51:0] dst52 = (52)'(src);
    logic[50:0] dst51 = (51)'(src);
    logic[49:0] dst50 = (50)'(src);

    logic[48:0] dst49 = (49)'(src);
    logic[47:0] dst48 = (48)'(src);
    logic[46:0] dst47 = (47)'(src);
    logic[45:0] dst46 = (46)'(src);
    logic[44:0] dst45 = (45)'(src);
    logic[43:0] dst44 = (44)'(src);
    logic[42:0] dst43 = (43)'(src);
    logic[41:0] dst42 = (42)'(src);
    logic[40:0] dst41 = (41)'(src);
    logic[39:0] dst40 = (40)'(src);

    logic[38:0] dst39 = (39)'(src);
    logic[37:0] dst38 = (38)'(src);
    logic[36:0] dst37 = (37)'(src);
    logic[35:0] dst36 = (36)'(src);
    logic[34:0] dst35 = (35)'(src);
    logic[33:0] dst34 = (34)'(src);
    logic[32:0] dst33 = (33)'(src);
    logic[31:0] dst32 = (32)'(src);
    logic[30:0] dst31 = (31)'(src);
    logic[29:0] dst30 = (30)'(src);

    logic[28:0] dst29 = (29)'(src);
    logic[27:0] dst28 = (28)'(src);
    logic[26:0] dst27 = (27)'(src);
    logic[25:0] dst26 = (26)'(src);
    logic[24:0] dst25 = (25)'(src);
    logic[23:0] dst24 = (24)'(src);
    logic[22:0] dst23 = (23)'(src);
    logic[21:0] dst22 = (22)'(src);
    logic[20:0] dst21 = (21)'(src);
    logic[19:0] dst20 = (20)'(src);

    logic[18:0] dst19 = (19)'(src);
    logic[17:0] dst18 = (18)'(src);
    logic[16:0] dst17 = (17)'(src);
    logic[15:0] dst16 = (16)'(src);
    logic[14:0] dst15 = (15)'(src);
    logic[13:0] dst14 = (14)'(src);
    logic[12:0] dst13 = (13)'(src);
    logic[11:0] dst12 = (12)'(src);
    logic[10:0] dst11 = (11)'(src);
    logic[9:0] dst10 = (10)'(src);

    logic[8:0] dst9 = (9)'(src);
    logic[7:0] dst8 = (8)'(src);
    logic[6:0] dst7 = (7)'(src);
    logic[5:0] dst6 = (6)'(src);
    logic[4:0] dst5 = (5)'(src);
    logic[3:0] dst4 = (4)'(src);
    logic[2:0] dst3 = (3)'(src);
    logic[1:0] dst2 = (2)'(src);
    logic dst1 = (1)'(src);
  endfuction

  function void tock_bN_offset();
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

    logic[63:0] src = 64'h1234567812345678;

    logic[7:0] dst0 = src[7:0];
    logic[7:0] dst1 = src[8:1];
    logic[7:0] dst2 = src[9:2];
    logic[7:0] dst3 = src[10:3];
    logic[7:0] dst4 = src[11:4];
    logic[7:0] dst5 = src[12:5];
    logic[7:0] dst6 = src[13:6];
    logic[7:0] dst7 = src[14:7];
    logic[7:0] dst8 = src[15:8];
    logic[7:0] dst9 = src[16:9];
  endfuction

  localparam int some_size1 = 64;
  localparam int some_size2 = 8;

  function void tock_bx_param();
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

    logic[some_size1-1:0] a = 10;
    logic[some_size2-1:0] b = (some_size2)'(a);

    logic[some_size2-1:0] b0 = a[some_size2+-1:0];
    logic[some_size2-1:0] b1 = a[some_size2+0:1];
    logic[some_size2-1:0] b2 = a[some_size2+1:2];
    logic[some_size2-1:0] b3 = a[some_size2+2:3];
    logic[some_size2-1:0] b4 = a[some_size2+3:4];
    logic[some_size2-1:0] b5 = a[some_size2+4:5];
    logic[some_size2-1:0] b6 = a[some_size2+5:6];
    logic[some_size2-1:0] b7 = a[some_size2+6:7];
    logic[some_size2-1:0] b8 = a[some_size2+7:8];
    logic[some_size2-1:0] b9 = a[some_size2+8:9];
  endfuction


  function void tock2();
    logic[31:0] a;
    logic b;
    logic[6:0] c;
    logic e;
    logic[6:0] f;

    logic[31:0] a = 32'hDEADBEEF;

    logic b = a[3]; //static bit extract with literal offset, width 1
    logic[6:0] c = a[9:3]; //static bit extract with literal offset, width N

    logic e = a[some_size1]; //static bit extract with variable offset, width 1
    logic[6:0] f = a[6 + some_size2 : some_size2]; //static bit extract with variable offset, width N
  endfuction



endmodule;

