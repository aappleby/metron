#include <stdio.h>

#include "Platform.h"
#include "Tests.h"
#include "Vpinwheel.h"
#include "submodules/CLI11/include/CLI/App.hpp"
#include "submodules/CLI11/include/CLI/Config.hpp"
#include "submodules/CLI11/include/CLI/Formatter.hpp"

//------------------------------------------------------------------------------

const char* instructions[38] = {
    "add", "addi", "and", "andi", "auipc", "beq",  "bge", "bgeu",
    "blt", "bltu", "bne", "jal",  "jalr",  "lb",   "lbu", "lh",
    "lhu", "lui",  "lw",  "or",   "ori",   "sb",   "sh",  "simple",
    "sll", "slli", "slt", "slti", "sltiu", "sltu", "sra", "srai",
    "srl", "srli", "sub", "sw",   "xor",   "xori"};

//------------------------------------------------------------------------------

uint64_t total_tocks = 0;
uint64_t total_time = 0;

TestResults test_instruction(const char* test_name, const int reps,
                             const int max_cycles) {
  TEST_INIT("Testing op %6s, %d reps", test_name, reps);

  char buf1[256];
  char buf2[256];
  sprintf(buf1, "+text_file=tests/rv_tests/%s.text.vh", test_name);
  sprintf(buf2, "+data_file=tests/rv_tests/%s.data.vh", test_name);
  const char* argv2[2] = {buf1, buf2};

  Verilated::commandArgs(2, argv2);

  int elapsed_cycles = 0;
  int test_result = -1;
  auto time_a = timestamp();

  //----------

  Vpinwheel top;

  for (int rep = 0; rep < reps; rep++) {
    top.tock_reset = 1;
    top.clock = 0;
    top.eval();
    top.clock = 1;
    top.eval();
    total_tocks++;
    top.tock_reset = 0;

    for (elapsed_cycles = 0; elapsed_cycles < max_cycles; elapsed_cycles++) {
      top.clock = 0;
      top.eval();
      top.clock = 1;
      top.eval();
      total_tocks++;

      if (top.bus_address == 0xfffffff0 && top.bus_write_enable) {
        test_result = top.bus_write_data;
        break;
      }
    }
  }

  //----------

  auto time_b = timestamp();
  total_time += time_b - time_a;

  if (elapsed_cycles == max_cycles) TEST_FAIL("TIMEOUT\n");
  if (test_result == 0) TEST_FAIL("FAIL %d @ %d\n", test_result, time);
  TEST_PASS();
}

//------------------------------------------------------------------------------

int main(int argc, const char** argv) {
  CLI::App app{"Simple test and benchmark for rvsimple"};

  int reps = 1;
  int max_cycles = 1000;

  app.add_option("-r,--reps", reps, "How many times to repeat the test");
  app.add_option("-m,--max_cycles", max_cycles,
                 "Maximum # cycles to simulate before timeout");
  CLI11_PARSE(app, argc, argv);

  LOG_B("Starting %s @ %d reps...\n", argv[0], reps);

  total_tocks = 0;
  total_time = 0;

  LOG_B("Testing...\n");
  TestResults results("main");
  for (int i = 0; i < 38; i++) {
    results << test_instruction(instructions[i], reps, max_cycles);
  }

  double rate = double(total_tocks) / double(total_time);
  LOG_B("Sim rate %f mhz\n", rate * 1000.0);

  return results.show_banner();
}
