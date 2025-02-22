#pragma once

#include <string>
#include <vector>

//------------------------------------------------------------------------------

struct Options {
  bool verbose = false;
  bool quiet = false;
  bool echo = false;
  bool dump = false;
  bool monochrome = false;
  std::vector<std::string> inc_paths;
  std::string src_name;
  std::string dst_name;
};

extern struct Options global_options;

//------------------------------------------------------------------------------

int main_new();
int main(int argc, char** argv);
