#include <stdio.h>

#include "metrolib/core/Platform.h"
#include "metrolib/core/Utils.h"
#include "metrolib/core/Tests.h"
#include "uart_top.h"

//------------------------------------------------------------------------------

void benchmark(const char* message_file) {
  const int cycles_per_bit = 3;
  const int repeat_msg = 1;
  const int cycle_max = 1000000000;

  uart_top<cycles_per_bit, repeat_msg> top(message_file);
  top.tock(1);

  auto time_a = timestamp();
  for (int cycle = 0; cycle < cycle_max; cycle++) {
    top.tock(0);
  }
  auto time_b = timestamp();

  double delta_sec = (double(time_b) - double(time_a)) / 1000000000.0;
  double rate = double(cycle_max) / delta_sec;
  LOG_B("Simulation rate %f Mhz\n", rate / 1000000.0);
}

//------------------------------------------------------------------------------

TestResults test_uart_metron(const char* message_file) {
  TEST_INIT("Metron UART simulation\n");

  const int cycles_per_bit = 3;
  uart_top<cycles_per_bit, 0> top(message_file);
  top.tock(1);

  LOG_B("========================================\n");

  for (int cycle = 0; cycle < 20000; cycle++) {
    bool old_valid = top.get_valid();
    top.tock(0);
    if (!old_valid && top.get_valid()) {
      LOG_B("%c", (uint8_t)top.get_data_out());
    }

    if (top.get_done()) {
      break;
    }
  }

  LOG_B("\n");
  LOG_B("========================================\n");

  EXPECT_EQ(0x0000b764, top.get_checksum(), "Verilator uart checksum fail");

  TEST_DONE();
}

//------------------------------------------------------------------------------

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: uart_cpp path/to/message.hex\n");
    return -1;
  }
  TestResults results;
  results << test_uart_metron(argv[1]);
  return results.show_result();
}

//------------------------------------------------------------------------------
