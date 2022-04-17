#include <stdio.h>
#include "Tests.h"

#define STRINGIZE1(M) #M
#define STRINGIZE2(M) STRINGIZE1(M)

#include STRINGIZE2(MT_HEADER)
#include STRINGIZE2(VL_HEADER)

TestResults test_lockstep() {
  TEST_INIT();

  MT_TOP mtop;
  VL_TOP vtop;

  for (int i = 0; i < 10; i++) {
    mtop.tock();

    vtop.clock = 0;
    vtop.eval();
    vtop.clock = 1;
    vtop.eval();

    EXPECT_EQ(mtop.result(), vtop.result, "Results should match");
    EXPECT_EQ(mtop.done(),   vtop.done,   "Done flag should match");

    if (mtop.done()) break;
  }

  TEST_DONE();
}

int main(int argc, char** argv) {
  TestResults results = test_lockstep();
  return results.test_fail ? -1 : 0;
}
