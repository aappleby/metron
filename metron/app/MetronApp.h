#pragma once

#include <string>

//------------------------------------------------------------------------------

struct Options {
  std::string src_name;
  std::string dst_name;
  bool verbose = false;
  bool quiet = false;
  bool echo = false;
  bool dump = false;
  bool monochrome = false;
  bool parse = false;
};

//------------------------------------------------------------------------------

int main_old(Options opts);
int main_new(Options opts);
int main(int argc, char** argv);
