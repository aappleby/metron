#include <stdio.h>

#include "Platform.h"
#include "Tests.h"
//#include "submodules/CLI11/include/CLI/App.hpp"
//#include "submodules/CLI11/include/CLI/Config.hpp"
//#include "submodules/CLI11/include/CLI/Formatter.hpp"
#include "pinwheel.h"

//------------------------------------------------------------------------------

const char* instructions[38] = {
  "add", "addi", "and", "andi", "auipc", "beq",  "bge", "bgeu",
  "blt", "bltu", "bne", "jal",  "jalr",  "lb",   "lbu", "lh",
  "lhu", "lui",  "lw",  "or",   "ori",   "sb",   "sh",  "simple",
  "sll", "slli", "slt", "slti", "sltiu", "sltu", "sra", "srai",
  "srl", "srli", "sub", "sw",   "xor",   "xori"
};

/*
const char* instructions[1] = {
  "simple"
};
*/

const int instruction_count = sizeof(instructions) / sizeof(instructions[0]);


//------------------------------------------------------------------------------

uint64_t total_tocks = 0;
uint64_t total_time = 0;

TestResults test_instruction(const char* test_name, const int reps,
                             int max_cycles) {
  TEST_INIT("Testing op %6s, %d reps: ", test_name, reps);

  char buf1[256];
  char buf2[256];
  sprintf(buf1, "+text_file=tests/rv_tests/%s.text.vh", test_name);
  sprintf(buf2, "+data_file=tests/rv_tests/%s.data.vh", test_name);
  const char* argv2[2] = {buf1, buf2};

  metron_init(2, argv2);

  int elapsed_cycles = 0;

  Pinwheel top;

  for (int rep = 0; rep < reps; rep++) {
    top.tock(1);
    auto time_a = timestamp();
    for (elapsed_cycles = 0; elapsed_cycles < max_cycles; elapsed_cycles++) {
      //top.dump();

      top.tock(0);
      total_tocks++;

      if (top.sig_p12.addr == 0xfffffff0 && top.sig_p12.mask) {
        if (top.sig_p12.data == 0) TEST_FAIL("FAIL @ %d\n", elapsed_cycles);
        if (rep == 0) {
          //LOG_B("pass on hart %d at cycle %d\n", top.sig_p12.hart, elapsed_cycles);
        }
        break;
      }
    }
    auto time_b = timestamp();
    total_time += time_b - time_a;
    if (elapsed_cycles == max_cycles) TEST_FAIL("TIMEOUT\n");
  }

  TEST_PASS();
}

//------------------------------------------------------------------------------

int main(int argc, const char** argv) {
  //CLI::App app{"Simple test and benchmark for rvsimple"};

  int reps = 1000;
  int max_cycles = 10000;

  //app.add_option("-r,--reps", reps, "How many times to repeat the test");
  //app.add_option("-m,--max_cycles", max_cycles,
  //               "Maximum # cycles to simulate before timeout");
  //CLI11_PARSE(app, argc, argv);

  LOG_B("Starting %s @ %d reps...\n", argv[0], reps);

  total_tocks = 0;
  total_time = 0;

  LOG_B("Testing...\n");
  TestResults results("main");
  for (int i = 0; i < instruction_count; i++) {
    results << test_instruction(instructions[i], reps, max_cycles);
  }

  double rate = double(total_tocks) / double(total_time);
  LOG_B("Sim rate %f mhz\n", rate * 1000.0);

  return results.show_banner();
}
