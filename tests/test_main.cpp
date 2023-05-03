#include <stdio.h>

#include "metrolib/core/Tests.h"
#include "metron/tools/metron_tools.h"
#include "test_utils.h"

TestResults test_utils();
TestResults test_logic();
TestResults test_uart();
TestResults test_ibex();
TestResults test_rvsimple();
TestResults test_build();

//------------------------------------------------------------------------------

int main(int argc, char** argv) {
  TestResults results;

  results << test_utils();  // this looks ok
  results << test_logic();  // this looks ok
  // results << test_ibex();

  return results.show_result();
}

//------------------------------------------------------------------------------
