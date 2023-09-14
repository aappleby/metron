`include "metron/tools/metron_tools.sv"

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
    src=0x1234567812345678;
    dst64=64'(src);
    dst63=63'(src);
    dst62=62'(src);
    dst61=61'(src);
    dst60=60'(src);

    dst59=59'(src);
    dst58=58'(src);
    dst57=57'(src);
    dst56=56'(src);
    dst55=55'(src);
    dst54=54'(src);
    dst53=53'(src);
    dst52=52'(src);
    dst51=51'(src);
    dst50=50'(src);

    dst49=49'(src);
    dst48=48'(src);
    dst47=47'(src);
    dst46=46'(src);
    dst45=45'(src);
    dst44=44'(src);
    dst43=43'(src);
    dst42=42'(src);
    dst41=41'(src);
    dst40=40'(src);

    dst39=39'(src);
    dst38=38'(src);
    dst37=37'(src);
    dst36=36'(src);
    dst35=35'(src);
    dst34=34'(src);
    dst33=33'(src);
    dst32=32'(src);
    dst31=31'(src);
    dst30=30'(src);

    dst29=29'(src);
    dst28=28'(src);
    dst27=27'(src);
    dst26=26'(src);
    dst25=25'(src);
    dst24=24'(src);
    dst23=23'(src);
    dst22=22'(src);
    dst21=21'(src);
    dst20=20'(src);

    dst19=19'(src);
    dst18=18'(src);
    dst17=17'(src);
    dst16=16'(src);
    dst15=15'(src);
    dst14=14'(src);
    dst13=13'(src);
    dst12=12'(src);
    dst11=11'(src);
    dst10=10'(src);

    dst9=9'(src);
    dst8=8'(src);
    dst7=7'(src);
    dst6=6'(src);
    dst5=5'(src);
    dst4=4'(src);
    dst3=3'(src);
    dst2=2'(src);
    dst1=1'(src);
    test_bN_ret = 0;
  end

  always_comb begin : test_bx_const
    src=0x1234567812345678;
    dst63=(63)'(src);
    dst62=(62)'(src);
    dst61=(61)'(src);
    dst60=(60)'(src);

    dst59=(59)'(src);
    dst58=(58)'(src);
    dst57=(57)'(src);
    dst56=(56)'(src);
    dst55=(55)'(src);
    dst54=(54)'(src);
    dst53=(53)'(src);
    dst52=(52)'(src);
    dst51=(51)'(src);
    dst50=(50)'(src);

    dst49=(49)'(src);
    dst48=(48)'(src);
    dst47=(47)'(src);
    dst46=(46)'(src);
    dst45=(45)'(src);
    dst44=(44)'(src);
    dst43=(43)'(src);
    dst42=(42)'(src);
    dst41=(41)'(src);
    dst40=(40)'(src);

    dst39=(39)'(src);
    dst38=(38)'(src);
    dst37=(37)'(src);
    dst36=(36)'(src);
    dst35=(35)'(src);
    dst34=(34)'(src);
    dst33=(33)'(src);
    dst32=(32)'(src);
    dst31=(31)'(src);
    dst30=(30)'(src);

    dst29=(29)'(src);
    dst28=(28)'(src);
    dst27=(27)'(src);
    dst26=(26)'(src);
    dst25=(25)'(src);
    dst24=(24)'(src);
    dst23=(23)'(src);
    dst22=(22)'(src);
    dst21=(21)'(src);
    dst20=(20)'(src);

    dst19=(19)'(src);
    dst18=(18)'(src);
    dst17=(17)'(src);
    dst16=(16)'(src);
    dst15=(15)'(src);
    dst14=(14)'(src);
    dst13=(13)'(src);
    dst12=(12)'(src);
    dst11=(11)'(src);
    dst10=(10)'(src);

    dst9=(9)'(src);
    dst8=(8)'(src);
    dst7=(7)'(src);
    dst6=(6)'(src);
    dst5=(5)'(src);
    dst4=(4)'(src);
    dst3=(3)'(src);
    dst2=(2)'(src);
    dst1=(1)'(src);
    test_bx_const_ret = 0;
  end

  always_comb begin : test_bN_offset
    src=0x1234567812345678;

    dst0=8'(src, 0);
    dst1=8'(src, 1);
    dst2=8'(src, 2);
    dst3=8'(src, 3);
    dst4=8'(src, 4);
    dst5=8'(src, 5);
    dst6=8'(src, 6);
    dst7=8'(src, 7);
    dst8=8'(src, 8);
    dst9=8'(src, 9);
    test_bN_offset_ret = 0;
  end


  always_comb begin : test_bx_param
    a=10;
    b=(some_size2)'(a);

    b0=(some_size2)'(a, 0);
    b1=(some_size2)'(a, 1);
    b2=(some_size2)'(a, 2);
    b3=(some_size2)'(a, 3);
    b4=(some_size2)'(a, 4);
    b5=(some_size2)'(a, 5);
    b6=(some_size2)'(a, 6);
    b7=(some_size2)'(a, 7);
    b8=(some_size2)'(a, 8);
    b9=(some_size2)'(a, 9);

    test_bx_param_ret = 0;
  end


  always_comb begin : test2
    a=0xDEADBEEF;

    b=1'(a, 3); //static bit extract with literal offset, width 1
    c=7'(a, 3); //static bit extract with literal offset, width N

    e=1'(a, some_size1); //static bit extract with variable offset, width 1
    f=7'(a, some_size2); //static bit extract with variable offset, width N

    test2_ret = 0;
  end



endmodule