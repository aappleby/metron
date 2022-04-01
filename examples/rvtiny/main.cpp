#include <stdio.h>

#include "metron/toplevel.h"
#include "tests/Tests.h"

//------------------------------------------------------------------------------

uint64_t total_tocks = 0;
uint64_t total_time = 0;

const int reps = 10000;
const int max_cycles = 1000;

TestResults test_instruction(const char* test_name, const int reps, const int timeout, bool verbose) {
  TEST_INIT("Testing op %6s, %d reps", test_name, reps);

  char buf1[256];
  char buf2[256];
  sprintf(buf1, "+text_file=rv_tests/%s.text.vh", test_name);
  sprintf(buf2, "+data_file=rv_tests/%s.data.vh", test_name);
  const char* argv2[2] = {buf1, buf2};

  metron_init(2, argv2);

  int time = 0;
  int result = -1;
  toplevel top;

  auto time_a = timestamp();
  for (int rep = 0; rep < reps; rep++) {
    top.tock(1);
    total_tocks++;
    for (time = 0; time < timeout; time++) {
      top.tock(0);
      total_tocks++;

      if (top.o_bus_write_enable && top.o_bus_address == 0xfffffff0) {
        result = top.o_bus_write_data;
        break;
      }
    }
  }
  auto time_b = timestamp();
  total_time += time_b - time_a;

  if (time == max_cycles) TEST_FAIL("TIMEOUT\n");
  if (!result) TEST_FAIL("FAIL %d @ %d\n", result, time);
  TEST_PASS();
}

//------------------------------------------------------------------------------

int main(int argc, const char** argv) {
  LOG_B("Starting %s @ %d reps...\n", argv[0], reps);

  const char* instructions[38] = {
    "add", "addi", "and", "andi", "auipc", "beq",  "bge", "bgeu",
    "blt", "bltu", "bne", "jal",  "jalr",  "lb",   "lbu", "lh",
    "lhu", "lui",  "lw",  "or",   "ori",   "sb",   "sh",  "simple",
    "sll", "slli", "slt", "slti", "sltiu", "sltu", "sra", "srai",
    "srl", "srli", "sub", "sw",   "xor",   "xori"};

  total_tocks = 0;
  total_time = 0;

  LOG_B("Testing...\n");
  TestResults results("main");
  for (int i = 0; i < 38; i++) {
    results += test_instruction(instructions[i], reps, max_cycles, true);
  }
  results.show_banner();

  double rate = double(total_tocks) / double(total_time);
  LOG_B("Sim rate %f mhz\n", rate * 1000.0);

  return 0;
}

//------------------------------------------------------------------------------
