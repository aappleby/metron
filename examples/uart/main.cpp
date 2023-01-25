#include <stdio.h>

#include "Platform.h"
#include "metron/uart_top.h"

void benchmark() {
  const int cycles_per_bit = 3;
  const int repeat_msg = 1;
  const int cycle_max = 1000000000;

  uart_top<cycles_per_bit, repeat_msg> top;
  top.tock(1);

  auto time_a = timestamp();
  for (int cycle = 0; cycle < cycle_max; cycle++) {
    top.tock(0);
  }
  auto time_b = timestamp();

  double delta_sec = (double(time_b) - double(time_a)) / 1000000000.0;
  double rate = double(cycle_max) / delta_sec;
  printf("Simulation rate %f Mhz\n", rate / 1000000.0);
 }

int main(int argc, char** arv) {
  printf("Metron simulation:\n");
  printf("================================================================================\n");

  const int cycles_per_bit = 3;
  uart_top<cycles_per_bit, 0> top;
  top.tock(1);

  for (int cycle = 0; cycle < 20000; cycle++) {
    bool old_valid = top.get_valid();
    top.tock(0);
    if (!old_valid && top.get_valid()) printf("%c", (uint8_t)top.get_data_out());

    if (top.get_done()) {
      printf("\n");
      printf("================================================================================\n");
      printf("%d\n", cycle);
      if (top.get_checksum() == 0x0000b764) {
        printf("All tests pass\n");
        return 0;
      }
    }
  }

  return 1;
}
