`include "metron/tools/metron_tools.sv"

// Most kinds of C++ enum declarations should work.

// bad
// enum { FOO, BAR, BAZ };
// typedef enum logic[1:0] { FOO=70, BAR=71, BAZ=72 } blem;
// typedef enum { FOO, BAR=0, BAZ=1 } blem;

// good
// OK enum { FOO, BAR, BAZ } blem;
// enum { FOO=0, BAR=1, BAZ=2 } blem;
// typedef enum { FOO, BAR, BAZ } blem;
// typedef enum { FOO=0, BAR=1, BAZ=2 } blem;
// typedef enum logic[1:0] { FOO, BAR, BAZ } blem;
// typedef enum logic[1:0] { FOO=0, BAR=1, BAZ=2 } blem;

// enum struct {} ? same as enum class

typedef enum {
  A0,
  B0,
  C0
} top_level_enum;

// clang-format off
module Module (
  // test1() ports
  output int test1_ret
);
 /*public:*/
  typedef enum { A1, /* random comment */ B1, C1 } simple_enum1;
  typedef enum { A2 = 0b01, B2 = 0x02, C2 = 3 } simple_enum2;

  enum { A3, B3, C3 } anon_enum_field1;
  enum { A4 = 0b01, B4 = 0x02, C4 = 3 } anon_enum_field2;

  typedef enum { A5, B5, C5 } enum_class1;
  typedef enum { A6 = 0b01, B6 = 0x02, C6 = 3 } enum_class2;

  // These should work in TreeSitter now
  typedef enum int { A7 = 0b01, B7 = 0x02, C7 = 3 } typed_enum;
  typedef enum logic[7:0] { A8 = 0b01, B8 = 0x02, C8 = 3 } sized_enum;

  always_comb begin : test1
    e1=A1;
    e2=B2;
    anon_enum_field1 = C3;
    anon_enum_field2 = A4;
    ec1=B5;
    ec2=C6;
    te1=A7;
    se1=B8;
    test1_ret = 1;
  end
endmodule