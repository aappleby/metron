#include <stdio.h>

#include "Tests.h"
#include "metron_tools.h"
#include "test_utils.h"

TestResults test_utils();
TestResults test_logic();
TestResults test_uart();
TestResults test_ibex();
TestResults test_rvsimple();
TestResults test_build();

//------------------------------------------------------------------------------

int main(int argc, char** argv) {
  TestResults results("main");

  results += test_utils();  // this looks ok
  results += test_logic();  // this looks ok
  // results += test_ibex();

  LOG_G("%s: %6d expect pass\n", __FUNCTION__, results.expect_pass);
  LOG_G("%s: %6d test pass\n", __FUNCTION__, results.test_pass);

  if (results.expect_fail)
    LOG_R("%s: %6d expect fail\n", __FUNCTION__, results.expect_fail);
  if (results.test_fail)
    LOG_R("%s: %6d test fail\n", __FUNCTION__, results.test_fail);

  return results.show_banner();
}

//------------------------------------------------------------------------------
