#include "metrolib/core/Tests.h"
#include "metron/metron_tools.h"

//------------------------------------------------------------------------------

TestResults test_logic_truncate() {
  TEST_INIT();

  logic<10> a = 0b1111111111;
  logic<5> b = 0;

  b = a + 0;
  EXPECT_EQ(0b11111, b, "Assignment after addition should compile and truncate.");

  TEST_DONE();
}

TestResults test_logic_add() {
  TEST_INIT();

  logic<8> a = 100;
  logic<8> b = 72;
  logic<8> c = a + b;

  EXPECT_EQ(172, c, "Bad addition?");

  TEST_DONE();
}

TestResults test_logic_cat() {
  TEST_INIT("test_logic_cat");

  logic<5> x = cat(logic<2>(0b10), logic<3>(0b101));

  EXPECT_EQ(1, x[4]);
  EXPECT_EQ(0, x[3]);
  EXPECT_EQ(1, x[2]);
  EXPECT_EQ(0, x[1]);
  EXPECT_EQ(1, x[0]);

  TEST_DONE();
}

TestResults test_logic_slice() {
  TEST_INIT();

  {
    logic<12> a = 0x00000000;

    a = 0;
    slice<8, 0>(a) = 0xFF;
    EXPECT_EQ(0x00FF, a);
    a = 0;
    slice<8, 1>(a) = 0xFF;
    EXPECT_EQ(0x01FE, a);
    a = 0;
    slice<8, 2>(a) = 0xFF;
    EXPECT_EQ(0x03FC, a);
    a = 0;
    slice<8, 3>(a) = 0xFF;
    EXPECT_EQ(0x07F8, a);
    a = 0;
    slice<8, 4>(a) = 0xFF;
    EXPECT_EQ(0x0FF0, a);
    a = 0;
    slice<8, 5>(a) = 0xFF;
    EXPECT_EQ(0x0FE0, a);
    a = 0;
    slice<8, 6>(a) = 0xFF;
    EXPECT_EQ(0x0FC0, a);
    a = 0;
    slice<8, 7>(a) = 0xFF;
    EXPECT_EQ(0x0F80, a);
    a = 0;
    slice<8, 8>(a) = 0xFF;
    EXPECT_EQ(0x0F00, a);
  }

  {
    logic<20> b = 0x0FF00;

    EXPECT_EQ(0x00FF, b12(b, 8));
    EXPECT_EQ(0x01FE, b12(b, 7));
    EXPECT_EQ(0x03FC, b12(b, 6));
    EXPECT_EQ(0x07F8, b12(b, 5));
    EXPECT_EQ(0x0FF0, b12(b, 4));
    EXPECT_EQ(0x0FE0, b12(b, 3));
    EXPECT_EQ(0x0FC0, b12(b, 2));
    EXPECT_EQ(0x0F80, b12(b, 1));
    EXPECT_EQ(0x0F00, b12(b, 0));
  }

  TEST_DONE();
}

TestResults test_logic_dup() {
  TEST_INIT();
  logic<3> boop = 0b101;
  logic<9> moop = dup<3>(boop);
  EXPECT_EQ(moop, 0b101101101);
  TEST_DONE();
}

//------------------------------------------------------------------------------

int main(int argc, char** argv) {
  TestResults results;

  results << test_logic_truncate();
  results << test_logic_add();
  results << test_logic_cat();
  results << test_logic_slice();
  results << test_logic_dup();

  return results.show_result();
}

//------------------------------------------------------------------------------
