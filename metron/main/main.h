#pragma once

#include <string>

//------------------------------------------------------------------------------

struct Options {
  bool verbose = false;
  bool quiet = false;
  bool echo = false;
  bool dump = false;
  bool monochrome = false;
  bool newlex = false;
  std::string inc_path;
  std::string src_name;
  std::string dst_name;
};

//------------------------------------------------------------------------------

int main_new(Options opts);
int main(int argc, char** argv);
