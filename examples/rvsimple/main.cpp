#include "CLI11/include/CLI/App.hpp"
#include "CLI11/include/CLI/Config.hpp"
#include "CLI11/include/CLI/Formatter.hpp"
#include "metrolib/core/Platform.h"
#include "metrolib/core/Tests.h"
#include "toplevel.h"

#include <stdio.h>

//------------------------------------------------------------------------------

const char* instructions[38] = {
  "add", "addi", "and", "andi", "auipc", "beq",  "bge", "bgeu",
  "blt", "bltu", "bne", "jal",  "jalr",  "lb",   "lbu", "lh",
  "lhu", "lui",  "lw",  "or",   "ori",   "sb",   "sh",  "simple",
  "sll", "slli", "slt", "slti", "sltiu", "sltu", "sra", "srai",
  "srl", "srli", "sub", "sw",   "xor",   "xori"
};

//------------------------------------------------------------------------------

double total_tocks = 0;
double total_time = 0;

TestResults test_instruction(
  std::string& inst_path,
  const char* test_name,
  const int reps,
  const int max_cycles
) {
  TEST_INIT("Testing op %6s, %d reps", test_name, reps);

  char code_filename[256];
  char data_filename[256];
  sprintf(code_filename, "%s/%s.code.vh", inst_path.c_str(), test_name);
  sprintf(data_filename, "%s/%s.data.vh", inst_path.c_str(), test_name);

  int elapsed_cycles = 0;
  int test_result = -1;

  //----------

  toplevel top(code_filename, data_filename);

  for (int rep = 0; rep < reps; rep++) {
    top.reset = 1;
    top.tock(0);
    top.reset = 0;

    auto time_a = timestamp();
    for (elapsed_cycles = 0; elapsed_cycles < max_cycles; elapsed_cycles++) {
      top.tock(0);
      total_tocks++;
      if (top.bus_address == 0xfffffff0 && top.bus_write_enable) {
        test_result = top.bus_write_data;
        break;
      }
    }
    auto time_b = timestamp();
    total_time += time_b - time_a;

  }

  //----------


  EXPECT_NE(max_cycles, elapsed_cycles, "TIMEOUT");
  EXPECT_NE(0, test_result, "FAIL %d @ %d\n", test_result, time);
  TEST_DONE();
}

//------------------------------------------------------------------------------

int main(int argc, const char** argv) {
  CLI::App app{"Simple test and benchmark for rvsimple"};

  std::string inst_path;
  int reps = 1;
  int max_cycles = 1000;

  app.add_option("inst_path", inst_path, "Path to the compiled instructions");
  app.add_option("-r,--reps", reps, "How many times to repeat the test");
  app.add_option("-m,--max_cycles", max_cycles,
                 "Maximum # cycles to simulate before timeout");
  CLI11_PARSE(app, argc, argv);

  LOG_B("Starting %s @ %d reps...\n", argv[0], reps);

  total_tocks = 0;
  total_time = 0;

  LOG_B("Testing...\n");
  TestResults results;
  for (int i = 0; i < 38; i++) {
    results << test_instruction(inst_path, instructions[i], reps, max_cycles);
  }

  double rate = double(total_tocks) / double(total_time);
  LOG_B("Total tocks %f\n",  total_tocks);
  LOG_B("Total time %f\n",   total_time);
  LOG_B("Sim rate %f mhz\n", rate / 1.0e6);

  return results.show_result();
}
