#include <stdio.h>
#include "metrolib/core/Tests.h"

#define STRINGIZE1(M) #M
#define STRINGIZE2(M) STRINGIZE1(M)

#include STRINGIZE2(MT_HEADER)
#include STRINGIZE2(VL_HEADER)

TestResults test_lockstep() {
  TEST_INIT();

  MT_TOP mtop;
  VL_TOP vtop;

  // Our tiny tests should simulate at a few tens of mhz at least, so a timeout
  // of 1 million cycles won't take too long.
  for (int i = 0; i < 1000000; i++) {
    mtop.tock();

    vtop.clock = 0;
    vtop.eval();
    vtop.clock = 1;
    vtop.eval();

    //printf("0x%08x 0x%08x\n", (uint32_t)mtop.result(), (uint32_t)vtop.result_ret);

    EXPECT_EQ(mtop.result(), vtop.result_ret, "Results should match");
    EXPECT_EQ(mtop.done(),   vtop.done_ret,   "Done flag should match");

    if (mtop.done()) break;
  }

  EXPECT_EQ(true, mtop.done(), "Test timed out");

  TEST_DONE();
}

int main(int argc, char** argv) {
  TestResults results = test_lockstep();
  return results.show_result();
}
